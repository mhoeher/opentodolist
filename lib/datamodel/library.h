#ifndef LIBRARY_H
#define LIBRARY_H

#include "libraryfactory.h"
#include "toplevelitemlist.h"

#include <QObject>
#include <QQmlListProperty>
#include <QSet>
#include <QString>
#include <QVector>


// Forward declaration:
class Image;
class LocalLibraryFactory;
class Note;
class TodoList;

class QFileSystemWatcher;
class QTimer;


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
    Q_PROPERTY(QQmlListProperty<TopLevelItem> items READ itemList NOTIFY itemsChanged)
    Q_PROPERTY(QStringList tags READ tags NOTIFY tagsChanged)
    
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
    Q_INVOKABLE Image* addImage(const QString &title, const QUrl &image);
    Q_INVOKABLE Image* addImage(const QString &image);
    Q_INVOKABLE Image* addImage(const QUrl &image);
    Q_INVOKABLE TodoList *addTodoList(const QString &title);
    TopLevelItemList items();
    QQmlListProperty<TopLevelItem> itemList();
    Q_INVOKABLE void deleteLibrary();
    
    QStringList tags() const;
    void setTags(const QStringList &tags);
    
signals:
    
    /**
     @brief The name of the library changed.
   */  
    void nameChanged();
    
    /**
     @brief The directory which the library encapsulates has changed.
   */
    void directoryChanged();
    
    /**
     @brief The list of top level items in a library changed.
   */
    void itemsChanged();
    
    /**
     @brief The library is deleted.
   */
    void libraryDeleted(Library *library);
    
    /**
     @brief The tags property has changed.
   */
    void tagsChanged();
    
    /**
       @brief One of the properties of the @p item changed.
     */
    void itemChanged(TopLevelItem *item);
    
    /**
       @brief An item has been added to the library.
     */
    void itemAdded();
    
    /**
       @brief The item ad the given @p index has been deleted.
     */
    void itemDeleted(int index);
    
public slots:
    
protected:
    
    explicit Library(const QString &name,
                     const QString &directory,
                     const LibraryFactory *factory,
                     QObject *parent = 0); 
    
private:
    
    static const int      UpdateChangedFilesDelayInMS;
    
    QString               m_name;
    QString               m_directory;
    const LibraryFactory *m_factory;
    TopLevelItemList      m_items;
    bool                  m_itemsLoaded;
    bool                  m_loadingItems;
    QSet<QString>         m_tags;
    QSet<QString>         m_changedFiles;
    QTimer               *m_updateChangedFilesTimer;
    
    QFileSystemWatcher   *m_fileSystemWatcher;
    
    QString itemPathFromTitle(const QString &title, const QString &itemType) const;
    void addItem(TopLevelItem *item);
    void addItemIfNoDuplicate(TopLevelItem* item);
    void loadItems();
    void deleteDanglingItems();
    void scanItems(const QString &startDir = QString());
    bool containsItem(const QUuid &uid) const;
    QString dirForItemType(const QString &itemType) const;
    
    static int itemListCount(QQmlListProperty<TopLevelItem> *property);
    static TopLevelItem *itemListAt(QQmlListProperty<TopLevelItem> *property, int index);
    
private slots:
    
    void onItemDeleted(QObject *item);
    void onTopLevelItemDeleted(Item *item);
    void watchRecursively();
    void onFileChanged(const QString &path);
    void updateChangedFiles();
    void rebuildTags();
    void handleItemChanged();
    
};


/**
   @brief A list of Library objects.
 */
typedef QVector<Library*> Libraries;

#endif // LIBRARY_H
