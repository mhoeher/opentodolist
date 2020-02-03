
#include <QDebug>
#include <QDir>
#include <QQmlEngine>
#include <QtConcurrent>
#include <QTimer>

#include "library.h"
#include "application.h"
#include "datastorage/cache.h"
#include "datastorage/getlibraryquery.h"
#include "datastorage/insertorupdateitemsquery.h"
#include "fileutils.h"
#include "toplevelitem.h"
#include "todolist.h"
#include "todo.h"
#include "task.h"
#include "sync/synchronizer.h"

#include "utils/jsonutils.h"


const QString Library::LibraryFileName = "library.json";

static Q_LOGGING_CATEGORY(log, "OpenTodoList.Library")


/**
 * @brief Constructor.
 */
LibraryCacheEntry::LibraryCacheEntry() :
    id(),
    data(),
    metaData(),
    valid(false)
{

}

bool LibraryCacheEntry::operator ==(const LibraryCacheEntry &other) const
{
    return id == other.id &&
            valid == other.valid &&
            toByteArray() == other.toByteArray();
}


/**
 * @brief Convert the enry to a byte array.
 */
QByteArray LibraryCacheEntry::toByteArray() const
{
    QVariantMap map;
    map["data"] = data;
    map["meta"] = metaData;
    map["type"] = Library::staticMetaObject.className();
    return QJsonDocument::fromVariant(map).toBinaryData();
}


/**
 * @brief Create an entry from a byte array.
 */
LibraryCacheEntry LibraryCacheEntry::fromByteArray(const QByteArray &data, const QByteArray &id)
{
    auto map = QJsonDocument::fromBinaryData(data).toVariant().toMap();
    LibraryCacheEntry result;
    if (map["type"].toString() == Library::staticMetaObject.className()) {
        result.id = QUuid(id);
        result.data = map["data"];
        result.metaData = map["meta"];
        result.valid = true;
    }
    return result;
}


/**
   @brief Set the name of the library.
 */
Library::Library(QObject* parent) : QObject(parent),
    m_uid(QUuid::createUuid()),
    m_name(),
    m_directory(),
    m_itemDataChanged(false)
{
    connect(this, &Library::uidChanged,
            this, &Library::changed);
    connect(this, &Library::nameChanged,
            this, &Library::changed);
    connect(this, &Library::tagsChanged,
            this, &Library::changed);
    connect(this, &Library::changed,
            this, &Library::onChanged);
}

Library::Library(const QString& directory, QObject* parent) :
    Library(parent)
{
    m_directory = directory;
}

Library::~Library()
{
}


bool Library::isValid() const
{
    return !m_directory.isEmpty() && QDir(m_directory).exists();
}


/**
 * @brief Creates a cache entry for the library.
 *
 * This method creates a representation of the library which can be written
 * to the cache.
 */
LibraryCacheEntry Library::encache() const
{
    LibraryCacheEntry result;
    result.id = m_uid;
    result.data = toMap();
    QVariantMap meta;
    meta["directory"] = FileUtils::toPersistedPath(m_directory);
    result.metaData = meta;
    result.valid = true;
    return result;
}


/**
 * @brief Construct a library from a cache entry.
 *
 * This constructs a library from the cache @p entry. The library will
 * be owned by the @p parent object.
 */
Library *Library::decache(const LibraryCacheEntry &entry, QObject *parent)
{
    Library *result = nullptr;
    if (entry.valid) {
        auto meta = entry.metaData.toMap();
        if (meta.contains("directory")) {
            auto path = meta["directory"].toString();
            path = FileUtils::fromPersistedPath(path);
            result = new Library(path, parent);
        } else {
            result = new Library(parent);
        }
        result->fromMap(entry.data.toMap());
        result->applyCalculatedData(entry.calculatedData.toMap());
        auto calculated = entry.calculatedData.toMap();
        result->applyCalculatedData(calculated);
    }
    return  result;
}


/**
 * @brief Construct a library from a cache entry.
 *
 * This constructs a library from the cache @p entry. The library will
 * be owned by the @p parent object.
 */
Library *Library::decache(const QVariant &entry, QObject *parent)
{
    return decache(entry.value<LibraryCacheEntry>(), parent);
}

void Library::setName(const QString &name)
{
    if ( m_name != name ) {
        m_name = name;
        emit nameChanged();
    }
}


/**
 * @brief Delete the library data on disk.
 *
 * This function deletes the files belonging to the library on disk.
 */
void Library::deleteLibrary()
{
    emit deletingLibrary(this);
    QString directory = m_directory;
    if (isValid()) {
        QtConcurrent::run([=](){
            auto years = Library::years(directory);
            for (auto year : years) {
                auto months = Library::months(directory, year);
                for (auto month : months) {
                    QDir dir(directory + "/" + year + "/" + month);
                    for (auto entry : dir.entryList(QDir::Files)) {
                        if (!dir.remove(entry)) {
                            qCWarning(log) << "Failed to remove file" << entry << "from"
                                               << dir.absolutePath();
                        }
                    }
                    dir.cdUp();
                    if (!dir.rmdir(month)) {
                        qCWarning(log) << "Failed to remove" << dir.absoluteFilePath(month);
                    }
                }
                if (!QDir(directory).rmdir(year)) {
                    qCWarning(log) << "Failed to remove" << (directory + "/" + year);
                }
            }
            QDir dir(directory);
            for (auto entry : dir.entryList(QDir::Files | QDir::Hidden)) {
                if (!dir.remove(entry)) {
                    qCWarning(log) << "Failed to remove" << entry
                                       << "from" << dir.absolutePath();
                }
            }
            dir.cdUp();
            auto basename = QFileInfo(directory).baseName();
            if (!dir.rmdir(basename)) {
                qCWarning(log) << "Failed to remove library directory" << dir.absolutePath();
            }
        });
    }
    emit libraryDeleted(this);
}

bool Library::load()
{
    bool result = false;
    if (isValid()) {
        QDir dir(m_directory);
        QString filename = dir.absoluteFilePath(LibraryFileName);
        bool ok;
        auto map = JsonUtils::loadMap(filename, &ok);
        if (ok) {
            fromMap(map);
            result = true;
        }
    }
    return result;
}

bool Library::save()
{
    bool result = false;
    if (isValid()) {
        QDir dir(m_directory);
        QString filename = dir.absoluteFilePath(LibraryFileName);
        result = JsonUtils::patchJsonFile(filename, toMap());
    }
    return result;
}


/**
 * @brief Get the sync log of the library.
 *
 * @sa Synchronizer::log()
 */
QVariant Library::syncLog()
{
    QVariantList result;
    if (isValid()) {
        QScopedPointer<Synchronizer> sync(Synchronizer::fromDirectory(m_directory));
        if (sync) {
            sync->loadLog();
            auto log = sync->log();
            for (auto entry : log) {
                QVariantMap map;
                map["time"] = entry.time;
                map["type"] = QVariant::fromValue(entry.type);
                map["message"] = entry.message;
                result << map;
            }

        }
    }
    return result;
}


/**
 * @brief Get the location of a new item.
 *
 * Returns an absolute path where a new item can be stored. Returns an empty
 * string if the library is invalid.
 */
QDir Library::newItemLocation() const
{
    QString result;
    if (isValid()) {
        QDir dir(m_directory);
        auto date = QDate::currentDate();
        auto subdir = QString("%1/%2");
        subdir = subdir.arg(date.year()).arg(date.month());
        if (dir.mkpath(subdir)) {
            result = dir.absoluteFilePath(subdir);
        }
    }
    return result;
}

/**
 * @brief Get the list of year entries.
 *
 * This searches the @p directory for a list if directories potentially holding
 * item data of a library. Returns the list of entries (relative to the directory).
 */
QStringList Library::years(const QString& directory)
{
    QStringList result;
    QDir dir(directory);
    QRegExp re("^\\d{4}$");
    if (!directory.isEmpty() && dir.exists()) {
        for (auto entry : dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
            if (re.indexIn(entry) == 0) {
                result << entry;
            }
        }
    }
    return result;
}

QStringList Library::months(const QString& directory, const QString& year)
{
    QStringList result;
    if (!directory.isEmpty()) {
        auto path = directory + "/" + year;
        QDir dir(path);
        QRegExp re("^[1-9][0-2]?$");
        if (dir.exists()) {
            for (auto entry : dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
                if (re.indexIn(entry) == 0) {
                    result << entry;
                }
            }
        }
    }
    return result;
}


/**
 * @brief Whether the library is in the default libraries location.
 *
 * This property indicates if the library is located in the default libraries
 * location.
 */
bool Library::isInDefaultLocation() const
{
    bool result = false;
    if (isValid()) {
        auto defaultLibrariesLocation = Library::defaultLibrariesLocation();
        defaultLibrariesLocation = QDir::cleanPath(defaultLibrariesLocation);

        auto thisDir = QFileInfo(m_directory).absoluteDir().path();

        result = thisDir == defaultLibrariesLocation;
    }
    return result;
}


/**
 * @brief The UID of the library.
 */
QUuid Library::uid() const
{
    return m_uid;
}

/**
 * @brief Get the list of tags in the library.
 *
 * This returns the list of tags - sorted alphabetically - used within the library.
 */
QStringList Library::tags() const
{
    return m_tags;
}

void Library::setTags(QStringList tags)
{
    m_tags = tags;
    emit tagsChanged();
}


/**
 * @brief Initialize the propertoes of the library from the JSON @p data.
 */
void Library::fromJson(const QByteArray data)
{
    auto doc = QJsonDocument::fromJson(data);
    if (doc.isObject()) {
        fromMap(doc.toVariant().toMap());
    }
}


/**
 * @brief Indicates if the library has a synchronizer.
 *
 * This property indicates if the library has a synchronizer set up.
 */
bool Library::hasSynchronizer() const
{
    bool result = false;
    if (isValid()) {
        auto sync = createSynchronizer();
        if (sync != nullptr) {
            result = true;
            delete sync;
        }
    }
    return result;
}


/**
 * @brief Create a synchronizer for the library.
 *
 * This creates an instance of the Synchronizer class (or more precise, a
 * concrete synchronizer depending on what the user set up for the library)
 * and returns it. The ownership of the object is with the caller.
 *
 * If specified, the synchronizer will be owned by the @p parent.
 *
 * If no synchronizer has been set up for the library, a nullptr is returned.
 *
 * @note The method also returns nullptrs if the library is invalid, i.e.
 * it has no local directory associated with it.
 */
Synchronizer *Library::createSynchronizer(QObject *parent) const
{
    if (isValid()) {
        return Synchronizer::fromDirectory(directory(), parent);
    } else {
        return nullptr;
    }
}



/**
 * @brief The cache the library is connected to.
 *
 * This is the Cache object the Library is connected to. The library
 * will automatically keep in sync with the set Cache.
 */
Cache *Library::cache() const
{
    return m_cache.data();
}


/**
 * @brief Set the cache the library is associated with.
 */
void Library::setCache(Cache *cache)
{
    if (m_cache != cache) {
        if (m_cache != nullptr) {
            disconnect(m_cache.data(), &Cache::dataChanged,
                       this, &Library::onCacheChanged);
            disconnect(this, &Library::changed,
                       this, &Library::onChanged);
        }
        m_cache = cache;
        if (m_cache != nullptr) {
            connect(m_cache.data(), &Cache::dataChanged,
                    this, &Library::onCacheChanged);
            connect(this, &Library::changed,
                    this, &Library::onChanged);
        }
    }
}

QVariant Library::toVariant()
{
    return toMap();
}

void Library::fromVariant(const QVariant &data)
{
    fromMap(data.toMap());
}

/**
 * @brief Get the location where libraries are stored by default.
 *
 * This returns the location where libraries are stored by default. If
 * the directory does not exist yet, it is created.
 */
QString Library::defaultLibrariesLocation()
{
    QString result;
    result = QStandardPaths::writableLocation(
                QStandardPaths::AppLocalDataLocation);
    result =  QDir(result).absolutePath();
    QDir(result).mkpath(".");
    return result;
}


/**
 * @brief Set the UID of the library.
 */
void Library::setUid(const QUuid& uid)
{
    if (m_uid != uid) {
        m_uid = uid;
        emit uidChanged();
    }
}

void Library::onCacheChanged()
{
    if (m_cache) {
        auto q = new GetLibraryQuery();
        q->setUid(m_uid);
        connect(q, &GetLibraryQuery::libraryLoaded,
                this, &Library::onLibraryDataLoadedFromCache,
                Qt::QueuedConnection);
        m_cache->run(q);
    }
}

void Library::onLibraryDataLoadedFromCache(const QVariant &entry)
{
    QSharedPointer<Library> lib(Library::decache(entry));
    if (lib != nullptr) {
        this->fromMap(lib->toMap());
        applyCalculatedData(
                    entry.value<LibraryCacheEntry>().calculatedData.toMap());
    }
}

void Library::onChanged()
{
    if (m_cache != nullptr) {
        auto q = new InsertOrUpdateItemsQuery();
        q->add(this, InsertOrUpdateItemsQuery::Save);
        m_cache->run(q);
    }
}

void Library::applyCalculatedData(const QVariantMap &properties)
{
    setTags(properties.value("tags", m_tags).toStringList());
}


QVariantMap Library::toMap() const
{
    QVariantMap result;
    result["uid"] = m_uid;
    result["name"] = m_name;
    return result;
}

void Library::fromMap(QVariantMap map)
{
    setUid(map.value("uid", m_uid).toUuid());
    setName(map.value("name", m_name).toString());
}
