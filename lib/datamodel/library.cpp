#include "library.h"

#include "image.h"
#include "note.h"
#include "todolist.h"
#include "fileutils.h"

#include <QDir>
#include <QDirIterator>
#include <QFileSystemWatcher>
#include <QMetaProperty>
#include <QQueue>
#include <QSet>
#include <QTimer>
#include <QUrl>

#include <QDebug>


const int Library::UpdateChangedFilesDelayInMS = 10000;

/**
   @brief Set the name of the library.
 */
void Library::setName(const QString &name)
{
    if ( m_name != name ) {
        m_name = name;
        emit nameChanged();
    }
}

/**
   @brief Set the directory for which the library returns objects.
 */
void Library::setDirectory(const QString &directory)
{
    if ( m_directory != directory ) {
        // TODO: Trigger search for new/changed items
        m_directory = directory;
        emit directoryChanged();
    }
}

/**
   @brief Return the arguments which are required to restore the library.
   
   This method returns the additional arguments which are required in order to restore the
   library. The arguments returned will be passed to the LibraryFactory::createLibrary()
   method to restore the library.
   
   Sub-classes shall extend this method to include their specific arguments.
 */
QVariantMap Library::saveArgs() const
{
    return QVariantMap();
}

Note *Library::addNote(const QString &title)
{
    auto result = new Note(itemPathFromTitle(title, Note::ItemType), this);
    Q_CHECK_PTR(result);
    result->setTitle(title);
    addItem(result);
    return result;
}

Image *Library::addImage(const QString &title, const QString &image)
{
    auto result = new Image(itemPathFromTitle(title, Image::ItemType), this);
    Q_CHECK_PTR(result);
    result->setTitle(title);
    result->setImage(image);
    addItem(result);
    return result;
}

Image *Library::addImage(const QString &title, const QUrl &image)
{
    return addImage(title, image.toLocalFile());
}

Image *Library::addImage(const QString &image)
{
    QFileInfo fi(image);
    return addImage(fi.fileName(), image);
}

Image *Library::addImage(const QUrl &image)
{
    return addImage(image.toLocalFile());
}

TodoList *Library::addTodoList(const QString &title)
{
    auto result = new TodoList(itemPathFromTitle(title, TodoList::ItemType), this);
    Q_CHECK_PTR(result);
    result->setTitle(title);
    addItem(result);
    return result;
}

TopLevelItemList Library::items()
{
    loadItems();
    return m_items;
}

QQmlListProperty<TopLevelItem> Library::itemList()
{
    return QQmlListProperty<TopLevelItem>(this, nullptr, itemListCount, itemListAt);
}

/**
   @brief Remove the library from the application.
   
   This method removes the library from the application. Basically, it emits the libraryDeleted()
   signal and then schedules the Library object for deletion.
   
   However, this will not remove the files on disk belonging to the library.
 */
void Library::deleteLibrary()
{
    emit libraryDeleted(this);
    deleteLater();
}

/**
   @brief Constructor.
   
   Creates a new library object with the given @p name and the @p directory. The library
   is created by the @p factory. The object is a child of the given @p parent.
 */
Library::Library(const QString &name, 
                 const QString &directory,
                 const LibraryFactory *factory,
                 QObject *parent) : QObject(parent),
    m_name( name ),
    m_directory( directory ),
    m_factory( factory ),
    m_items(),
    m_itemsLoaded(false),
    m_loadingItems(false),
    m_tags(),
    m_changedFiles(),
    m_updateChangedFilesTimer(new QTimer(this)),
    m_fileSystemWatcher(new QFileSystemWatcher(nullptr))
{
    Q_ASSERT(m_fileSystemWatcher != nullptr);
    connect(m_fileSystemWatcher, &QFileSystemWatcher::fileChanged,
            this, &Library::onFileChanged);
    connect(m_fileSystemWatcher, &QFileSystemWatcher::directoryChanged,
            this, &Library::onFileChanged);
    watchRecursively();
    m_updateChangedFilesTimer->setInterval(UpdateChangedFilesDelayInMS);
    m_updateChangedFilesTimer->setSingleShot(true);
    connect(m_updateChangedFilesTimer, &QTimer::timeout,
            this, &Library::updateChangedFiles);
}

/**
   @brief Returns the list of tags used by items in the library.
   @return 
 */
QStringList Library::tags() const
{
    QStringList result = m_tags.toList();
    result.sort();
    return result;
}

/**
   @brief Sets the tags belonging to the library.
   
   This sets the library's tags. Note that this is used for tag caching (i.e. the tags
   are set by the application when the library is restored). Any changes in the library's
   list of items will cause the tag list to be re-calculated.
 */
void Library::setTags(const QStringList& tags)
{
    for (auto tag : tags) 
    {
        m_tags.insert(tag);
    }
    emit tagsChanged();
}

QString Library::itemPathFromTitle(const QString &title, const QString &itemType) const
{
    QString baseDir = dirForItemType(itemType);
    QString tpl = baseDir + "/" + Item::titleToDirectoryName(title);
    int i = 0;
    QString result = tpl;
    while (QDir(result).exists()) {
        result = tpl + " - " + QString::number(i++);
    }
    return result;
}

void Library::addItem(TopLevelItem *item)
{
    if (!m_loadingItems) {
        item->commitItem();
    }
    connect(item, &Item::itemDeleted, this, &Library::onTopLevelItemDeleted);
    connect(item, &QObject::destroyed, this, &Library::onItemDeleted);
    connect(item, &TopLevelItem::tagsChanged, this, &Library::rebuildTags);
    {
        // Listen to property changed (required for models):
        QMetaMethod targetSlot;
        auto myMeta = metaObject();
        for (int i = 0; i < myMeta->methodCount(); ++i)
        {
            auto method = myMeta->method(i);
            if (method.name() == "handleItemChanged")
            {
                targetSlot = method;
                break;
            }
        }
        if (targetSlot.isValid())
        {
            auto meta = item->metaObject();
            for (int i = 0; i < meta->propertyCount(); ++i)
            {
                QMetaProperty property = meta->property(i);
                if (property.notifySignal().isValid())
                {
                    connect(item, property.notifySignal(),
                            this, targetSlot);
                }
            }
        }
    }
    m_items.append(item);
    emit itemAdded();
    emit itemsChanged();
    rebuildTags();
}

/**
   @brief Adds an item if to the library if it is not a duplicate.
   
   This will use addItem() on the passed @p item if it is not already in the
   library. Otherwise, the item will be deleted.
   
   @note The caller must not assume that the pointer to the item is still valid after the
         call.
 */
void Library::addItemIfNoDuplicate(TopLevelItem *item)
{
    Q_CHECK_PTR(item);
    if (containsItem(item->uid()))
    {
        delete item;
    } 
    else    
    {
        addItem(item);
    }
}

void Library::loadItems()
{
    if (!m_itemsLoaded) {
        m_itemsLoaded = true;
        scanItems();
    }
}

void Library::deleteDanglingItems()
{
    for (auto item : m_items) {
        if (item->isDangling()) {
            item->deleteItem();
        }
    }
}

void Library::scanItems(const QString &startDir)
{
    m_loadingItems = true;
    QSet<QString> itemDirs;
    for (auto item : m_items) {
        itemDirs.insert(item->directory());
    }
    
    QQueue<QString> dirs;
    QSet<QString> visitedDirs;
    if (startDir.isEmpty()) {
        dirs.enqueue(QFileInfo(m_directory).canonicalFilePath());
    } else {
        dirs.enqueue(QFileInfo(startDir).canonicalFilePath());
    }
    while (!dirs.isEmpty()) {
        QString dir = dirs.dequeue();
        visitedDirs.insert(dir);
        if (itemDirs.contains(dir)) {
            continue;
        }
        if (Item::isItemDirectory<Note>(dir)) {
            auto note = new Note(dir, this);
            Q_CHECK_PTR(note);
            addItemIfNoDuplicate(note);
        } else if (Item::isItemDirectory<Image>(dir)) {
            auto image = new Image(dir, this);
            Q_CHECK_PTR(image);
            addItemIfNoDuplicate(image);
        } else if(Item::isItemDirectory<TodoList>(dir)) {
            auto todoList = new TodoList(dir, this);
            Q_CHECK_PTR(todoList);
            addItemIfNoDuplicate(todoList);
        } else {
            QDir d(dir);
            for (auto entry : d.entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
                QString newPath = QFileInfo(d.absoluteFilePath(entry)).canonicalFilePath();
                if (!visitedDirs.contains(newPath)) {
                    dirs.enqueue(newPath);
                }
            }
        }
    }
    m_loadingItems = false;
    rebuildTags();
}

bool Library::containsItem(const QUuid &uid) const
{
    for (auto item : m_items) {
        if (item->uid() == uid) {
            return true;
        }
    }
    return false;
}

QString Library::dirForItemType(const QString &itemType) const
{
    QString baseDir = m_directory;
    QString appDir = baseDir + "/OpenTodoList";
    int i = 0;
    while (QFile::exists(appDir) && !QDir(appDir).exists() && !QFileInfo(appDir).isWritable()) {
        appDir = baseDir + "/OpenTodoList - " + QString::number(i++);
    }
    QString typeDir = appDir + "/" + itemType;
    i = 0;
    while (QFile::exists(typeDir) && !QDir(appDir).exists() && !QFileInfo(appDir).isWritable()) {
        typeDir = appDir + "/" + itemType + " - " + QString::number(i);
    }
    return typeDir;
}

int Library::itemListCount(QQmlListProperty<TopLevelItem> *property)
{
    Library *_this = dynamic_cast<Library*>(property->object);
    Q_CHECK_PTR(_this);
    return _this->items().size();
}

TopLevelItem* Library::itemListAt(QQmlListProperty<TopLevelItem> *property, int index)
{
    Library *_this = dynamic_cast<Library*>(property->object);
    Q_CHECK_PTR(_this);
    return _this->items().at(index);
}

void Library::onItemDeleted(QObject* item)
{
    Q_CHECK_PTR(item);
    int index = m_items.indexOf(reinterpret_cast<TopLevelItem*>(item));
    if (index >= 0) {
        emit itemDeleted(index);
        emit itemsChanged();
    }
}

void Library::onTopLevelItemDeleted(Item *item)
{
    Q_CHECK_PTR(item);
    for (int i = 0; i < m_items.size(); ++i) {
        if (m_items.at(i) == item) {
            m_items.removeAt(i);
            item->deleteLater();
            emit itemDeleted(i);
            emit itemsChanged();
            return;
        }
    }
}

void Library::watchRecursively()
{
    auto watchedFiles = QSet<QString>::fromList(m_fileSystemWatcher->files());
    auto watchedDirs = QSet<QString>::fromList(m_fileSystemWatcher->directories());
    auto existingFiles = QSet<QString>();
    auto existingDirs = QSet<QString>();
    QDirIterator it(m_directory, QDirIterator::Subdirectories);
    while (it.hasNext()) {
        auto entry = it.next();
        QFileInfo fi(entry);
        if (fi.isDir()) {
            existingDirs.insert(entry);
            if (!watchedDirs.contains(entry)) {
                m_fileSystemWatcher->addPath(entry);
            }
        } else if (fi.isFile()) {
            existingFiles.insert(entry);
            if (!watchedFiles.contains(entry)) {
                m_fileSystemWatcher->addPath(entry);
            }
        }
    }
    auto oldDirs = watchedDirs - existingDirs;
    auto oldFiles = watchedFiles - existingFiles;
    if (!oldDirs.isEmpty()) {
        m_fileSystemWatcher->removePaths(oldDirs.toList());
    }
    if (!oldFiles.isEmpty()) {
        m_fileSystemWatcher->removePaths(oldFiles.toList());
    }
}

void Library::onFileChanged(const QString &path)
{
    m_changedFiles.insert(path);
    m_updateChangedFilesTimer->start();
}

void Library::updateChangedFiles()
{
    for (QString path : m_changedFiles)
    {
        QFileInfo fi(path);
        QString absolutePath = fi.absoluteFilePath();
        if (FileUtils::isSubDirOrFile(m_directory, absolutePath)) {
            for (Item *item : m_items) {
                if (FileUtils::isSubDirOrFile(item->directory(), absolutePath)) {
                    item->handleFileChanged(absolutePath);
                    return;
                }
            }
            
            deleteDanglingItems();
            scanItems(m_directory);
        }
    }
    m_changedFiles.clear();
}

void Library::rebuildTags()
{
    if (!m_loadingItems) {
        QSet<QString> newTags;
        for (auto item : m_items)
        {
            for (auto tag : item->tags())
            {
                newTags.insert(tag);
            }
        }
        if (m_tags != newTags)
        {
            m_tags = newTags;
            emit tagsChanged();
        }
    }
}

void Library::handleItemChanged()
{
    TopLevelItem *item = dynamic_cast<TopLevelItem*>(sender());
    if (item != nullptr)
    {
        emit itemChanged(item);
    }
}