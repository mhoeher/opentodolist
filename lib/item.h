#ifndef ITEM_H
#define ITEM_H

#include <QObject>
#include <QString>
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
  Q_PROPERTY(QString directory READ directory CONSTANT)
public:
  
  static const QStringList PersistentProperties;
  static const QString ItemType;
  
  explicit Item(const QString &directory = QString(),
                const QString &itemType = ItemType,
                const QStringList &persistentProperties = QStringList(),
                QObject *parent = 0);
  
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
     @brief The persistence file name into which the item's properties are stored.
   */
  QString persistenceFilename() const { return ".opentodolist" + m_itemType.toLower(); }
  
signals:

  /**
     @brief The title of the item has changed.
   */  
  void titleChanged();
  
  
public slots:
  
protected:
  
  explicit Item(bool loadItem,
                const QString &directory = QString(),
                const QString &itemType = ItemType,
                const QStringList &persistentProperties = QStringList(),
                QObject *parent = 0);
  void initializeItem();
  
  virtual void loadItem();
  virtual void saveItem();
  
  
private:
  
  QString     m_title;
  QString     m_directory;
  QUuid       m_uid;
  QString     m_itemType;
  QStringList m_persistentProperties;
  bool        m_loadingSettings;
};

QDebug operator<<(QDebug debug, const Item *item);

#endif // ITEM_H
