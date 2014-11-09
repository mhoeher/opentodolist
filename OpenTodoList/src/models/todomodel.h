#ifndef TODOMODEL_H
#define TODOMODEL_H

#include "core/opentodolistinterfaces.h"
#include "datamodel/todo.h"
#include "datamodel/todolist.h"
#include "database/database.h"
#include "database/storagequery.h"

#include <QAbstractListModel>
#include <QPointer>
#include <QSet>
#include <QTimer>
#include <QVector>

namespace OpenTodoList {
namespace DataModel {
class Todo;
}
}

namespace OpenTodoList {

namespace Models {

using namespace DataModel;
using namespace DataBase;

/**
   @brief A model for interacting with todos

   This class implements a model on the todos in the application. It provides
   several filter capabilities to exactly specify which todos to include in the
   model. The model is inexpensive in the means that it will create
   run time objects for only the todos that are matched by the filter criteria.
 */
class TodoModel : public QAbstractListModel
{
    Q_OBJECT
    Q_ENUMS(TodoSortMode)
    Q_ENUMS( QueryType )
    Q_PROPERTY(OpenTodoList::DataBase::Database* database READ database WRITE setDatabase NOTIFY databaseChanged)
    Q_PROPERTY( OpenTodoList::DataModel::TodoList* todoList READ todoList WRITE setTodoList NOTIFY todoListChanged)
    Q_PROPERTY( QString filter READ filter WRITE setFilter NOTIFY filterChanged )
    Q_PROPERTY( bool showDone READ showDone WRITE setShowDone NOTIFY showDoneChanged )
    Q_PROPERTY( bool showDeleted READ showDeleted WRITE setShowDeleted NOTIFY showDeletedChanged )
    Q_PROPERTY( bool hideUndeleted READ hideUndeleted WRITE setHideUndeleted NOTIFY hideUndeletedChanged)
    Q_PROPERTY( QDateTime maxDueDate READ maxDueDate WRITE setMaxDueDate NOTIFY maxDueDateChanged)
    Q_PROPERTY( QDateTime minDueDate READ minDueDate WRITE setMinDueDate NOTIFY minDueDateChanged)
    Q_PROPERTY( int count READ rowCount NOTIFY countChanged )
    Q_PROPERTY( TodoSortMode sortMode READ sortMode WRITE setSortMode NOTIFY sortModeChanged )
    Q_PROPERTY( TodoSortMode backendSortMode READ backendSortMode WRITE setBackendSortMode NOTIFY backendSortModeChanged)
    Q_PROPERTY( int limitOffset READ limitOffset WRITE setLimitOffset NOTIFY limitOffsetChanged)
    Q_PROPERTY( int limitCount READ limitCount WRITE setLimitCount NOTIFY limitCountChanged)

public:

    enum TodoSortMode {
        SortTodoByName,
        SortTodoByPriority,
        SortTodoByDueDate,
        SortTodoByWeight
    };

    /**
       @brief Compares two TodoList objects
     */
    class Comparator {
    public:
        explicit Comparator(TodoSortMode sortMode);
        int operator () ( DataModel::Todo* const &first, DataModel::Todo* const &second ) const;
    private:
        TodoSortMode m_sortMode;
    };

    explicit TodoModel(QObject *parent = 0);
    virtual ~TodoModel();

    // QAbstractItemModel interface
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
    virtual QVariant data(const QModelIndex &index, int role) const;
    virtual void sort(int column, Qt::SortOrder order);

    QString filter() const;
    void setFilter(const QString &filter);

    bool showDone() const;
    void setShowDone(bool showDone);

    bool showDeleted() const;
    void setShowDeleted(bool showDeleted);

    QDateTime maxDueDate() const;
    void setMaxDueDate(const QDateTime &maxDueDate);

    QDateTime minDueDate() const;
    void setMinDueDate(const QDateTime &minDueDate);

    bool hideUndeleted() const;
    void setHideUndeleted(bool hideUndeleted);

    Q_INVOKABLE DataModel::Todo* get( int index ) const;

    TodoSortMode sortMode() const;
    void setSortMode(const TodoSortMode &sortMode);

    TodoSortMode backendSortMode() const;
    void setBackendSortMode(const TodoSortMode &backendSortMode);

    int limitOffset() const;
    void setLimitOffset(int limitOffset);

    int limitCount() const;
    void setLimitCount(int limitCount);

    DataModel::TodoList *todoList() const;
    void setTodoList(DataModel::TodoList *todoList);

    Database *database() const;
    void setDatabase(Database *database);

signals:

    void queryTypeChanged();
    void todoListChanged();
    void filterChanged();
    void showDoneChanged();
    void showDeletedChanged();
    void hideUndeletedChanged();
    void maxDueDateChanged();
    void minDueDateChanged();
    void changed();
    void countChanged();
    void sortModeChanged();
    void backendSortModeChanged();
    void limitOffsetChanged();
    void limitCountChanged();

    void databaseChanged();

public slots:

    void sort();
    void refresh();

private:

    Database                   *m_database;
    QVector< DataModel::Todo* > m_todos;
    QSet< QString >             m_loadedTodos;
    QSet< QString >             m_newLoadedTodos;
    QTimer                     *m_updateTimer;

    QPointer<DataModel::TodoList> m_todoList;
    QPointer<DataModel::Todo>     m_parentTodo;
    QString                       m_filter;
    bool                          m_showDone;
    bool                          m_showDeleted;
    bool                          m_hideUndeleted;
    QDateTime                     m_maxDueDate;
    QDateTime                     m_minDueDate;
    TodoSortMode                  m_sortMode;
    TodoSortMode                  m_backendSortMode;
    int                           m_limitOffset;
    int                           m_limitCount;

    bool todoIsVisible( const Todo* todo ) const;

private slots:

    void addTodo( const QVariant &todo );
    void refilter();

};

} /* Models */

} /* OpenTodoList */

#endif // TODOMODEL_H
