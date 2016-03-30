#include "item.h"

#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QJsonDocument>
#include <QtGlobal>
#include <QTimer>
#include <QVariant>
#include <QVariantMap>


/**
  @brief The list of persistent attributes of the Item class.
  
  This is the list of property names of the Item class, which is saved when Item::saveItem()
  and loaded when Item::loadItem() are called.
  */
const QStringList Item::PersistentProperties = {"title"};

/**
   @brief The 
 */
const QString Item::ItemType = "Item";

/**
   @brief Constructor.
   
   Creates a new Item which is stored in the given @p directory. When using Item::saveItem() and
   Item::loadItem() are used, the list of @p persistentProperties of the item instance is
   saved and loaded. The item will be a child of the given @p parent.
   
   @note To construct an invalid item, pass in an empty string.
 */
Item::Item(const QString &directory, const QString &itemType, const QStringList &persistentProperties, QObject *parent) : 
  QObject(parent),
  m_title(),
  m_directory(QFileInfo(directory).absoluteFilePath()),
  m_uid(),
  m_itemType(itemType),
  m_persistentProperties(persistentProperties + PersistentProperties),
  m_loadingSettings(false),
  m_modified(false),
  m_deleted(false),
  m_readonly(false)
{
  initializeItem();
}

Item::~Item()
{
}

/**
   @brief Saves the item data to disk.

   This method is used to save the item information to disk. The base implementation writes to the
   Item::persistenceFilename() file in the Item::directory(). All persistent properties passed in
   the constructor call will be written to the file. Sub-classes should implement the
   Item::saveItemData() method to write additional files as necessary.
   
   Usually, this method does not need to be called manually. It is either called directly or
   indirectly (via an event) by saveItem(). However, in a test case it might be required
   to manually flush any scheduled write operations before deleting an item.
*/
void Item::commitItem()
{
  if (m_modified) {
    if (!m_deleted && isValid() && !m_readonly) {
      if (!m_loadingSettings) {
        QDir itemDir(m_directory);
        if (!itemDir.exists()) {
          itemDir.mkpath(m_directory);
        }
        QFile file(itemMainSettingsFile());
        if (file.open(QIODevice::WriteOnly)) {
          QVariantMap properties;
          for (auto property : m_persistentProperties) {
            properties[property] = this->property(qUtf8Printable(property));
          }
          properties["uid"] = m_uid;
          QJsonDocument doc = QJsonDocument::fromVariant(properties);
          file.write(doc.toJson());
          file.close();
          saveItemData();
        } else {
          qWarning().nospace()
              << "Failed to open " << file.fileName() << " for item" << this << ": " << 
                 file.errorString();
        }
      }
    }
    m_modified = false;
  }
}

/**
   @brief Deletes the item permanently.
   
   This method can be used to permanently delete an item. Calling this will delete any data on
   disk belonging to the item. After the call to this method, the item should be deleted (or at
   least considered disposed).
 */
bool Item::deleteItem()
{
  if (!m_deleted && isValid() && !readonly()) {
    QDir dir(m_directory);
    m_deleted = true;
    emit itemDeleted(this);
    if (dir.exists()) {
      return dir.removeRecursively();
    }
  }
  return false;
}

/**
   @brief Returns true if the item has been deleted on disk.
 */
bool Item::isDangling() const
{
  return !QFile(itemMainSettingsFile()).exists();
}

/**
   @brief Sets the title of the item.
 */
void Item::setTitle(const QString &title)
{
  if ( m_title != title ) {
    m_title = title;
    emit titleChanged();
    saveItem();
  }
}

/**
   @brief Returns the file name of the main file where the item data is stored.
 */
QString Item::itemMainSettingsFile() const
{
  return m_directory + "/" + persistenceFilename();
}

/**
   @brief Returns true of the @p directory is a storage of the given @p itemType.
   
   This method can be used to test if a given directory contains data of
   a certain item type:
   
   @code
   if (Item::isItemDirectory("/home/otl/sampletodo/", Todo::ItemType) {
     // Load the todo...
   }
   @endcode
 */
bool Item::isItemDirectory(const QString &directory, const QString &itemType)
{
  return QFile::exists(directory + "/" + persistenceFilename(itemType));
}

/**
   @brief Constructor.
   
   This is an overloaded constructor. If @p loadItem is true, the constructor will call
   the initializeItem() method. If a sub-class passes false, it shall call initializeItem() on
   its own.
 */
Item::Item(bool loadItem, 
           const QString &directory, 
           const QString &itemType, 
           const QStringList &persistentProperties, 
           QObject *parent) :
  QObject(parent),
  m_title(),
  m_directory(QFileInfo(directory).absoluteFilePath()),
  m_uid(),
  m_itemType(itemType),
  m_persistentProperties(persistentProperties + PersistentProperties),
  m_loadingSettings(false),
  m_deleted(false),
  m_readonly(false)
{
  if (loadItem) {
    initializeItem();
  }
}

/**
   @brief Initializes the item at startup
   
   This method is called by the constructors of the item class (or derived classes) in
   order to initialize the item - both runtime information as well as configuration files on
   disk.
 */
void Item::initializeItem()
{
  Q_ASSERT(!m_itemType.contains(QRegExp("[^a-zA-Z]")));
  Q_ASSERT(!m_itemType.isEmpty());
  QFileInfo fi(itemMainSettingsFile());
  if (fi.exists()) {
    loadItem();
    m_readonly = !fi.isWritable();
  } else {
    m_uid = QUuid::createUuid();
    saveItem(SaveItemImmediately);
  }
}

/**
   @brief Load the item data from disk.
   
   This method is used to restore item information from disk. Basically, this will open the file
   Item::persistenceFilename() in the Item::directory() and read the contained properties. The
   generic implementation will read back all persistent properties as given in the constructor of
   the Item.
   
   Sub-classes should implement the Item::loadItemData() to load additional data from the item's
   directory.
   
   @sa Item::saveItem()
   @sa Item::commitItem()
 */
void Item::loadItem()
{
  if ( isValid() ) {
    m_loadingSettings = true;
    QFileInfo fi(itemMainSettingsFile());
    if (fi.exists()) {
      QFile file(fi.filePath());
      if (file.open(QIODevice::ReadOnly)) {
        QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
        QVariant v = doc.toVariant();
        if (v.type() != QVariant::Map) {
          qWarning() << "Expected" << file.fileName() << "to be a map structure, but got" <<
                        v.typeName();
        } else {
          QVariantMap map = v.toMap();
          for (auto property : m_persistentProperties) {
            setProperty(qUtf8Printable(property), 
                        map.value(property, this->property(qUtf8Printable(property))));
          }
          m_uid = map.value("uid", m_uid).toUuid();
        }
        loadItemData(); // Load further item data if required
      } else {
        qWarning().nospace() 
            << "Failed to load" << fi.filePath() << "of" << this
                   << ": " << file.errorString();
      }
    } else {
      qDebug() << "File" << fi.filePath() << "of" << this << "does not exist.";
    }
    m_loadingSettings = false;
  }
}

/**
   @brief Saves the item data to disk.
   
   This method triggers saving of the item to disk. It takes a @p strategy which determines how
   the item is to be saved. If the strategy is Item::SaveItemImmediately, the item is
   saved immediately. If the strategy is Item::SaveItemLater, then saving of the item
   is delayed until control returns to the event loop. The default strategy is Item::SaveItemLater.
   This allows to set multipe properties of an item instance and save them to disk in one operation
   later on.
 */
void Item::saveItem(SaveItemStrategy strategy)
{
  if (!m_loadingSettings) {
    m_modified = true;
    switch (strategy) {
    case SaveItemImmediately:
      commitItem();
      break;
    case SaveItemLater:
      QTimer::singleShot(0, this, &Item::commitItem);
      break;
    default:
      qFatal("Unexpected Item::SaveItemStrategy received: %d", strategy);
      break;
    }
  }
}

/**
   @brief Returns a version of the @p title which can be used as directory name.
 */
QString Item::titleToDirectoryName(const QString &title)
{
  QString copy = title;
  copy.replace(QRegExp("[^\\w\\s]"), ""); // Remove bad chars
  copy.replace(QRegExp("\\s+"), " "); // Replace consecutive whitespace with a single one
  copy.replace(QRegExp("^\\s*"), ""); // Remove leading space
  copy.replace(QRegExp("\\s*$"), ""); // Remove trailing space
  if (copy.isEmpty() || copy == " ") {
    copy = "Item";
  }
  return copy;
}

/**
   @brief Handle a change of a file.
   
   This method is called by the parent of an item (i.e. either the library
   or container item to which the item belongs) when the file specified by the
   given @p filename changed (i.e. it was removed, added or modified).
 */
void Item::handleFileChanged(const QString &filename)
{
  reload();
}

/**
   @brief Re-loads the item.
   
   Calling this causes the item to re-load any data from disk.
 */
void Item::reload()
{
  QFileInfo fi(itemMainSettingsFile());
  if (fi.exists()) {
    loadItem();
    m_loadingSettings = true;
    emit reloaded();
    m_loadingSettings = false;
  } else {
    // Main settings file gone -> assume the item has been deleted
    deleteItem();
  }
}

/**
   @brief Load additional item data.
   
   This method is called by Item::loadItem(). Sub-classes of Item can override it to load additional
   data when the item is loaded.
 */
void Item::loadItemData()
{
  // Nothing to be done here
}

/**
   @brief Save additional item data.
   
   This method is called by Item::commitItem(). Sub-classes of Item can override it to save
   additional data when the item is saved.
 */
void Item::saveItemData()
{
  // Nothing to be done here
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
