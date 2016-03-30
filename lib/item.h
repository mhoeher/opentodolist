#ifndef ITEM_H
#define ITEM_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QUuid>


/**
   @brief Base class for all items in a library.
   
   This class is the base class for any kind of item which can either directly or indirectly
   be stored in a Library. It provides the common interface any such item must have.
 */
class Item : public QObject
{
  Q_OBJECT
  Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY titleChanged)
  Q_PROPERTY(QString itemType READ itemType CONSTANT)
  Q_PROPERTY(QString directory READ directory CONSTANT)
  Q_PROPERTY(bool isValid READ isValid CONSTANT)
  Q_PROPERTY(bool readonly READ readonly CONSTANT)
  
public:
  
  static const QStringList PersistentProperties;
  static const QString ItemType;
  
  explicit Item(const QString &directory = QString(),
                const QString &itemType = ItemType,
                const QStringList &persistentProperties = QStringList(),
                QObject *parent = 0);
  virtual ~Item();
  
  void commitItem();
  
  Q_INVOKABLE bool deleteItem();
  
  /**
     @brief Returns true if the item is valid.
   */
  bool isValid() { return !m_directory.isEmpty(); }
  
  /**
     @brief Returns true if the item is read-only.
   */
  bool readonly() { return m_readonly; }
  
  bool isDangling() const;
  
  /**
     @brief The title of the item as shown in the user interface.
   */
  QString title() const { return m_title; }
  void setTitle(const QString &title);
  
  /**
     @brief The directory the item is stored in.
   */
  QString directory() const { return m_directory; }
  
  /**
     @brief The globally unique ID of the item.
   */
  QUuid uid() const { return m_uid; }
  
  /**
     @brief The type name of the item. Used to generate the persistence file name.
   */
  QString itemType() const { return m_itemType; }
  
  /**
     @brief The persistence file name used by items of the @p itemType.
   */
  static QString persistenceFilename(const QString &itemType) 
    { return itemType.toLower() + ".opentodolist"; }
  
  /**
     @brief The persistence file name into which the item's properties are stored.
   */
  QString persistenceFilename() const { return Item::persistenceFilename(m_itemType); }
  
  QString itemMainSettingsFile() const;
  
  static bool isItemDirectory(const QString &directory, const QString &itemType);
  
  /**
    @brief Check if the @p directory is the data directory of a item type.
    
    This method can be used to test if a given directory is a data directory
    of the item type T:
    
    @code
    if (Item::isItemDirectory<Todo>(directory)) {
      // Load the todo...
    }
    @endcode
   */
  template<typename T>
  static bool isItemDirectory(const QString &directory) 
    { return Item::isItemDirectory(directory, T::ItemType); }
  
  static QString titleToDirectoryName(const QString &title);
  
  virtual void handleFileChanged(const QString &filename);
  
  
public slots:
  
  void reload();
  
signals:

  /**
     @brief The title of the item has changed.
   */  
  void titleChanged();
  
  /**
     @brief The item has been deleted.
     
     This signal is emitted when Item::deleteItem() is used to delete the item.
   */
  void itemDeleted(Item *item);
  
  /**
     @brief The item has been reloaded.
     
     This signal is emitted to indicate that the item has been reloaded. This is
     the case if the files on disk belonging to the item have changed.
   */
  void reloaded();
  
protected:
  
  enum SaveItemStrategy {
    SaveItemImmediately,
    SaveItemLater
  };
  
  explicit Item(bool loadItem,
                const QString &directory = QString(),
                const QString &itemType = ItemType,
                const QStringList &persistentProperties = QStringList(),
                QObject *parent = 0);
  void initializeItem();
  
  void loadItem();
  void saveItem(SaveItemStrategy strategy = SaveItemLater);
  
  virtual void loadItemData();
  virtual void saveItemData();
  
  
private:
  
  QString     m_title;
  QString     m_directory;
  QUuid       m_uid;
  QString     m_itemType;
  QStringList m_persistentProperties;
  bool        m_loadingSettings;
  bool        m_modified;
  bool        m_deleted;
  bool        m_readonly;
  
};

QDebug operator<<(QDebug debug, const Item *item);

#endif // ITEM_H
