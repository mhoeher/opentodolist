#ifndef LIBRARY_H
#define LIBRARY_H

#include "libraryfactory.h"
#include "toplevelitemlist.h"

#include <QObject>
#include <QString>
#include <QVector>


// Forward declaration:
class Image;
class LocalLibraryFactory;
class Note;
class TodoList;


/**
   @brief A container for items.
   
   The Library class is a container for TopLevelItem objects. Each library has a directory, for
   which it loads and makes the contained items available. Sub-classes can be created in order
   to extend the basic container interface, i.e. to enable synchronization with an external
   server.
   
   Library objects cannot directly be instantiated. Instead, they are created via
   a LibraryFactory.
 */
class Library : public QObject
{
  Q_OBJECT
  Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
  Q_PROPERTY(QString directory READ directory WRITE setDirectory NOTIFY directoryChanged)
  Q_PROPERTY(const LibraryFactory* factory READ factory CONSTANT)
  
  friend class LocalLibraryFactory;
 
public:
  
  /**
     @brief The name of the libary as used in the user interface.
   */
  QString name() const      { return m_name; }
  void setName( const QString &name );
  
  /**
     @brief Returns the directory which the library encapsulates.
   */
  QString directory() const { return m_directory; }
  void setDirectory( const QString &directory );
  
  virtual QVariantMap saveArgs() const;

  /**
     @brief The factory which was used to create the library.
   */  
  const LibraryFactory* factory() const { return m_factory; }
  
  Q_INVOKABLE Note* addNote(const QString &title);
  Q_INVOKABLE Image* addImage(const QString &title, const QString &image);
  Q_INVOKABLE TodoList *addTodoList(const QString &title);
  TopLevelItemList items();
  
signals:

  /**
     @brief The name of the library changed.
   */  
  void nameChanged();
  
  /**
     @brief The directory which the library encapsulates has changed.
   */
  void directoryChanged();
  
public slots:
  
protected:
  
  explicit Library(const QString &name,
                   const QString &directory,
                   const LibraryFactory *factory,
                   QObject *parent = 0); 
  
private:
  
  QString               m_name;
  QString               m_directory;
  const LibraryFactory *m_factory;
  TopLevelItemList      m_items;
  bool                  m_itemsLoaded;
  
  QString itemPathFromTitle(const QString &title) const;
  void addItem(TopLevelItem *item);
  void loadItems();
  bool containsItem(const QUuid &uid) const;
  
private slots:
  
  void onTopLevelItemDeleted(Item *item);
  
};


/**
   @brief A list of Library objects.
 */
typedef QVector<Library*> Libraries;

#endif // LIBRARY_H
