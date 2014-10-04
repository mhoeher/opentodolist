#ifndef TODOLISTMODEL_H
#define TODOLISTMODEL_H

#include "datamodel/todolistlibrary.h"
#include "core/opentodolistinterfaces.h"
#include "datamodel/todolist.h"

#include "database/storagequery.h"

#include <QAbstractListModel>
#include <QPointer>
#include <QSet>
#include <QVariantMap>
#include <QVector>

namespace OpenTodoList {
namespace DataModel {
class TodoListLibrary;
}
}

namespace OpenTodoList {

namespace Models {

class TodoListModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY( OpenTodoList::DataModel::TodoListLibrary* library READ library WRITE setLibrary NOTIFY libraryChanged )
    Q_PROPERTY( int count READ rowCount NOTIFY countChanged )

public:

    /**
       @brief Compares two TodoList objects
     */
    class Comparator {
    public:
        Comparator() {}
        int operator () ( DataModel::TodoList* const &first,
                          DataModel::TodoList* const &second ) const;
    };

    explicit TodoListModel(QObject *parent = 0);
    virtual ~TodoListModel();

    DataModel::TodoListLibrary *library() const;
    void setLibrary( DataModel::TodoListLibrary* library );

    // QAbstractItemModel interface
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
    virtual QVariant data(const QModelIndex &index, int role) const;
    virtual void sort(int column, Qt::SortOrder order);

signals:

    void libraryChanged();
    void beginUpdate();
    void endUpdate();
    void countChanged();

public slots:

    void update();
    void sort();

private:

    QVector< DataModel::TodoList* >      m_todoLists;
    QPointer<DataModel::TodoListLibrary> m_library;
    QSet< QString >           m_newTodoLists;
    QSet< QString >           m_loadedTodoLists;
    bool                      m_updateNeeded;

    //static QString todoListId(const QString &backend, const TodoListStruct &list );

private slots:

    void handleTodoListDeleted( QObject *list );

    void triggerUpdate();
    //void addTodoList( const QString &backend, const TodoListStruct &list );
    void removeExtraLists();

};

/**
   @brief Query all todo lists

   This query is used to get all todo lists from the database.
 */
class TodoListQuery : public DataBase::StorageQuery
{
    Q_OBJECT

public:

    explicit TodoListQuery();
    virtual ~TodoListQuery();

    // TodoListStorageQuery interface
    virtual void beginRun();
    virtual bool query(QString &query, QVariantMap &args);
    virtual void recordAvailable(const QVariantMap &record);
    virtual void endRun();

signals:

    void clearList();
    //void addTodoList( const QString &backend, const TodoListStruct &list );
    void finished();

};

} /* Models */

} /* OpenTodoList */

#endif // TODOLISTMODEL_H
