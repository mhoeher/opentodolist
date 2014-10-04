#ifndef TODOMODEL_H
#define TODOMODEL_H

#include "core/opentodolistinterfaces.h"
#include "datamodel/todo.h"
#include "datamodel/todolist.h"
#include "database/storagequery.h"

#include <QAbstractListModel>
#include <QPointer>
#include <QSet>
#include <QTimer>
#include <QVector>

namespace OpenTodoList {
namespace DataModel {
class TodoListLibrary;
}
}

namespace OpenTodoList {

namespace Models {

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
    Q_PROPERTY( OpenTodoList::DataModel::TodoListLibrary* library READ library WRITE setLibrary NOTIFY libraryChanged )
    Q_PROPERTY( QueryType queryType READ queryType WRITE setQueryType NOTIFY queryTypeChanged )
    Q_PROPERTY( OpenTodoList::DataModel::TodoList* todoList READ todoList WRITE setTodoList NOTIFY todoListChanged)
    Q_PROPERTY( OpenTodoList::DataModel::Todo* parentTodo READ parentTodo WRITE setParentTodo NOTIFY parentTodoChanged)
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
        SortTodoByPercentageDone,
        SortTodoByDueDate
    };

    /**
       @brief The type of listing the model shows
     */
    enum QueryType {
        InvalidQuery, //!< The model shows nothing
        QueryTopLevelTodosInTodoList, //!< The model shows top level todos in a todo list
        QuerySubTodosOfTodo, //!< The model shows sub-todos of a given todo
        QuerySearchTodos, //!< Search within all todos
        QueryFilterTodos //!< Like search but do not produce empty lists when search string is empty
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

    DataModel::TodoListLibrary *library() const;
    void setLibrary( DataModel::TodoListLibrary *library );

    // QAbstractItemModel interface
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
    virtual QVariant data(const QModelIndex &index, int role) const;
    virtual void sort(int column, Qt::SortOrder order);

    QueryType queryType() const;
    void setQueryType(const QueryType &queryType);

    DataModel::Todo *parentTodo() const;
    void setParentTodo(DataModel::Todo *parentTodo);

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
signals:

    void libraryChanged();
    void queryTypeChanged();
    void todoListChanged();
    void parentTodoChanged();
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

public slots:

    void update();
    void sort();

private:

    QueryType                   m_queryType;
    QVector< DataModel::Todo* > m_todos;
    QPointer<DataModel::TodoListLibrary> m_library;
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

    static QString idForTodo( const QString &backend, const QString &todoId );

private slots:

    void triggerUpdate();
    void addTodo( const QString &backend, const ITodo *todo );
    void removeExtraneousTodos();

    void handleTodoDeleted( QObject *todo );
};

/**
   @brief Query todos from the application storage
 */
class TodoStorageQuery : public DataBase::StorageQuery
{
    Q_OBJECT
public:

    explicit TodoStorageQuery();
    virtual ~TodoStorageQuery();

    QString backend() const;
    void setBackend(const QString &backend);

    QString todoListId() const;
    void setTodoListId(const QString &todoListId);

    QString parentTodoId() const;
    void setParentTodoId(const QString &parentTodoId);

    TodoModel::QueryType queryType() const;
    void setQueryType(const TodoModel::QueryType &queryType);

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

    // TodoListStorageQuery interface
    virtual void beginRun();
    virtual bool query(QString &query, QVariantMap &args);
    virtual void recordAvailable(const QVariantMap &record);
    virtual void endRun();

    TodoModel::TodoSortMode backendSortMode() const;
    void setBackendSortMode(const TodoModel::TodoSortMode &backendSortMode);

    int limitOffset() const;
    void setLimitOffset(int limitOffset);

    int limitCount() const;
    void setLimitCount(int limitCount);

signals:

    //void todoAvailable( const QString &backend, const TodoStruct &todo );
    void finished();

private:

    TodoModel::QueryType m_queryType;
    QString              m_backend;
    QString              m_todoListId;
    QString              m_parentTodoId;
    QString              m_filter;
    bool                 m_showDone;
    bool                 m_showDeleted;
    bool                 m_hideUndeleted;
    QDateTime            m_maxDueDate;
    QDateTime            m_minDueDate;
    TodoModel::TodoSortMode   m_backendSortMode;
    int                  m_limitOffset;
    int                  m_limitCount;

};

} /* Models */

} /* OpenTodoList */

#endif // TODOMODEL_H
