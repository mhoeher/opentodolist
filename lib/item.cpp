#include "item.h"

#include "fileutils.h"

#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QJsonDocument>
#include <QtGlobal>
#include <QVariant>
#include <QVariantMap>


/**
 * @brief Creates an invalid item.
 *
 * This constructs an invalid item, i.e. an item with no file associated on disk with it.
 *
 * @sa isValid()
 */
Item::Item(QObject* parent) :
    QObject(parent),
    m_filename(),
    m_title(),
    m_uid(QUuid::createUuid()),
    m_loading(false)
{
}

/**
 * @brief Create an item.
 *
 * This constructor creates an item which stores its data in the @p filename.
 */
Item::Item(const QString& filename, QObject* parent) :
    Item(parent)
{
    m_filename = filename;
    load();
}

/**
 * @brief Destructor.
 */
Item::~Item()
{
}

/**
 * @brief Delete the item.
 *
 * This method can be called to permanently delete the item from the library.
 * This will remove the data file on disk representing the item (if such a file exists)
 * and emit the itemDeleted() signal afterwards.
 *
 * @note This is a virtual method. Sub-classes of the Item class might override
 *       it to add custom delete handling.
 */
bool Item::deleteItem()
{
    bool result = false;
    if (isValid()) {
        QFile file(m_filename);
        if (file.exists()) {
            result = file.remove();
        }
    }
    emit itemDeleted(this);
    return result;
}

/**
 * @brief Load the item from disk.
 *
 * This method reads back the item data from disk.
 */
bool Item::load()
{
    bool result = false;
    QFile file(m_filename);
    if (file.exists()) {
        if (file.open(QIODevice::ReadOnly)) {
            auto data = file.readAll();
            QJsonParseError error;
            auto doc = QJsonDocument::fromJson(data, &error);
            if (error.error == QJsonParseError::NoError) {
                auto loading = m_loading;
                m_loading = true;
                fromMap(doc.toVariant().toMap());
                m_loading = loading;
                result = true;
            } else {
                qWarning() << "Failed to parse" << m_filename << ":" << error.errorString();
            }
        } else {
            qWarning() << "Failed to open" << m_filename << "for reading:" << file.errorString();
        }
    }
    return result;
}

/**
 * @brief Save the item data back to disk.
 *
 * Use this to trigger a save operation of the item back to disk. This should in particular
 * be called in property setters of sub-classes of the item class.
 *
 * @note This method will have no effect during a load() or any other operation during
 * which the item is restored from its persisted state.
 */
bool Item::save()
{
    bool result = false;
    if (!m_loading) {
        if (isValid()) {
            QFile file(m_filename);
            QVariantMap map;
            if (file.open(QIODevice::ReadOnly)) {
                auto doc = QJsonDocument::fromJson(file.readAll());
                map = doc.toVariant().toMap();
                file.close();
            }

            // Merge new map into existing to preserve attributes from newer versions
            auto newMap = toMap();
            for (auto key : newMap.keys()) {
                map.insert(key, newMap[key]);
            }

            auto doc = QJsonDocument::fromVariant(map);
            auto json = doc.toJson(QJsonDocument::Indented);

            if (file.open(QIODevice::WriteOnly)) {
                file.write(json);
                file.close();
                result = true;
            } else {
                qWarning() << "Failed to open" << m_filename << "for writing:"
                           << file.errorString();
            }
        }
    }
    return result;
}

/**
 * @brief Save the item to a QVariant for persistence.
 *
 * This saves the Item to a QVariant representation. This representation can be used to
 * create an instance of the item in one thread and transfer it to another.
 */
QVariant Item::toVariant() const
{
    QVariantMap result;
    result["filename"] = m_filename;
    result["data"] = toMap();
    result["itemType"] = itemType();
    return result;
}

/**
 * @brief Restore the item from a QVariant.
 *
 * Restores the item from the @p data created using toVariant().
 */
void Item::fromVariant(QVariant data)
{
    QVariantMap map = data.toMap();
    setFilename(map.value("filename", m_filename).toString());
    auto loading = m_loading;
    m_loading = true;
    fromMap(map.value("data", QVariantMap()).toMap());
    m_loading = loading;
}

/**
 * @brief Saves the properties of the item to a QVariantMap.
 */
QVariantMap Item::toMap() const
{
    QVariantMap result;
    result["uid"] = m_uid;
    result["title"] = m_title;
    result["weight"] = m_weight;
    return result;
}

/**
 * @brief Restores the properties of the item from the @p map.
 */
void Item::fromMap(QVariantMap map)
{
    setUid(map.value("uid", m_uid).toUuid());
    setTitle(map.value("title", m_title).toString());
    setWeight(map.value("weight", m_weight).toDouble());
}

/**
 * @brief The weight of the item.
 *
 * This property holds the weight of the item. The weight property is used to
 * manually sort items. This is done by ordering the items in a list by their
 * weight and allowing the user to set the weight.
 */
double Item::weight() const
{
    return m_weight;
}

/**
 * @brief Set the weight property.
 */
void Item::setWeight(double weight)
{
    if (!qFuzzyCompare(m_weight, weight)) {
        m_weight = weight;
        emit weightChanged();
        save();
    }
}

/**
 * @brief The directory which contains the item's data.
 *
 * This returns the directory where the item's data files are stored. If the
 * item is invalid, returns an empty string.
 */
QString Item::directory() const
{
    if (isValid()) {
        return QFileInfo(m_filename).path();
    }
    return QString();
}

/**
   @brief Sets the title of the item.
 */
void Item::setTitle(const QString &title)
{
    if ( m_title != title ) {
        m_title = title;
        emit titleChanged();
        save();
    }
}

/**
 * @brief The type of the item.
 *
 * This property holds the type name of the item (e.g. "TodoList" or "Task"). It
 * is written into the item's data file and used when reading information back.
 */
QString Item::itemType() const
{
    return metaObject()->className();
}

void Item::setUid(const QUuid& uid)
{
    // Note: This shall not trigger a save operation! Change of uid shall happen
    // only on de-serialization, hence, no need to trigger a save right now.
    if (m_uid != uid) {
        m_uid = uid;
        emit uidChanged();
    }
}

void Item::setFilename(const QString& filename)
{
    // Note: Same as for setUid(), this shall not trigger a save() operation.
    if (m_filename != filename) {
        m_filename = filename;
        emit filenameChanged();
    }
}

/**
   @brief Write an item to a debug stream.
 */
QDebug operator<<(QDebug debug, const Item *item)
{
    QDebugStateSaver saver(debug);
    Q_UNUSED(saver);

    if (item) {
        debug.nospace() << item->itemType() << "(\"" << item->title() << "\" [" << item->uid() << "])";
    } else {
        debug << "Item(nullptr)";
    }
    return debug;
}
