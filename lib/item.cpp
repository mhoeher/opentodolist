#include "item.h"

#include <QDebug>
#include <QFileInfo>
#include <QJsonDocument>
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
 */
Item::Item(const QString &directory, const QString &itemType, const QStringList &persistentProperties, QObject *parent) : 
  QObject(parent),
  m_title(),
  m_directory(directory),
  m_uid(),
  m_itemType(itemType),
  m_persistentProperties(persistentProperties + PersistentProperties),
  m_loadingSettings(false)
{
  initializeItem();
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
  m_directory(directory),
  m_uid(),
  m_itemType(itemType),
  m_persistentProperties(persistentProperties + PersistentProperties),
  m_loadingSettings(false)
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
  QFileInfo fi(m_directory + "/" + persistenceFilename());
  if (fi.exists()) {
    loadItem();
  } else {
    m_uid = QUuid::createUuid();
    saveItem();
  }
}

/**
   @brief Load the item data from disk.
   
   This method is used to restore item information from disk. Basically, this will open the file
   Item::persistenceFilename() in the Item::directory() and read the contained properties. The
   generic implementation will read back all persistent properties as given in the constructor of
   the Item. Sub-classes can extend the functionality to read further files if required.
   
   @sa Item::saveItem()
 */
void Item::loadItem()
{
  m_loadingSettings = true;
  QFileInfo fi(m_directory + "/" + persistenceFilename());
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

/**
   @brief Saves the item data to disk.
   
   This method is used to save the item information to disk. The base implementation writes to the
   Item::persistenceFilename() file in the Item::directory(). All persistent properties passed in
   the constructor call will be written to the file. Sub-classes can extend the functionality and
   e.g. write to additional files.
 */
void Item::saveItem()
{
  if (!m_loadingSettings) {
    QFile file(m_directory + "/" + persistenceFilename());
    if (file.open(QIODevice::WriteOnly)) {
      QVariantMap properties;
      for (auto property : m_persistentProperties) {
        properties[property] = this->property(qUtf8Printable(property));
      }
      properties["uid"] = m_uid;
      QJsonDocument doc = QJsonDocument::fromVariant(properties);
      file.write(doc.toJson());
      file.close();
    } else {
      qWarning().nospace()
                 << "Failed to open " << file.fileName() << " for item" << this << ": " << 
                    file.errorString();
    }
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
