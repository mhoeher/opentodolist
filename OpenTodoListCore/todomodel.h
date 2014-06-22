#ifndef TODOMODEL_H
#define TODOMODEL_H

#include "opentodolistinterfaces.h"
#include "todo.h"
#include "todoliststoragequery.h"

#include <QAbstractListModel>
#include <QPointer>
#include <QSet>
#include <QVector>

class TodoListLibrary;

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
    Q_ENUMS( QueryType )
    Q_PROPERTY( TodoListLibrary* library READ library WRITE setLibrary NOTIFY libraryChanged )
    Q_PROPERTY( QueryType queryType READ queryType WRITE setQueryType NOTIFY queryTypeChanged )
    Q_PROPERTY( TodoList* todoList READ todoList WRITE setTodoList NOTIFY todoListChanged)
    Q_PROPERTY( Todo* parentTodo READ parentTodo WRITE setParentTodo NOTIFY parentTodoChanged)
    Q_PROPERTY( QString filter READ filter WRITE setFilter NOTIFY filterChanged )
    Q_PROPERTY( bool showDone READ showDone WRITE setShowDone NOTIFY showDoneChanged )
    Q_PROPERTY( bool showDeleted READ showDeleted WRITE setShowDeleted NOTIFY showDeletedChanged )
    Q_PROPERTY( bool hideUndeleted READ hideUndeleted WRITE setHideUndeleted NOTIFY hideUndeletedChanged)
    Q_PROPERTY( QDateTime maxDueDate READ maxDueDate WRITE setMaxDueDate NOTIFY maxDueDateChanged)
    Q_PROPERTY( int count READ rowCount NOTIFY countChanged )
    Q_PROPERTY( Todo::TodoSortMode sortMode READ sortMode WRITE setSortMode NOTIFY sortModeChanged )

public:

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

    explicit TodoModel(QObject *parent = 0);
    virtual ~TodoModel();

    TodoListLibrary *library() const;
    void setLibrary( TodoListLibrary *library );

    // QAbstractItemModel interface
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
    virtual QVariant data(const QModelIndex &index, int role) const;
    virtual void sort(int column, Qt::SortOrder order);

    QueryType queryType() const;
    void setQueryType(const QueryType &queryType);

    TodoList *todoList() const;
    void setTodoList(TodoList *todoList);

    Todo *parentTodo() const;
    void setParentTodo(Todo *parentTodo);

    QString filter() const;
    void setFilter(const QString &filter);

    bool showDone() const;
    void setShowDone(bool showDone);

    bool showDeleted() const;
    void setShowDeleted(bool showDeleted);

    QDateTime maxDueDate() const;
    void setMaxDueDate(const QDateTime &maxDueDate);

    bool hideUndeleted() const;
    void setHideUndeleted(bool hideUndeleted);

    Q_INVOKABLE Todo* get( int index ) const;

    Todo::TodoSortMode sortMode() const;
    void setSortMode(const Todo::TodoSortMode &sortMode);

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
    void changed();
    void countChanged();
    void sortModeChanged();

public slots:

    void update();
    void sort();

private:

    QueryType                 m_queryType;
    QVector< Todo* >          m_todos;
    QPointer<TodoListLibrary> m_library;
    QSet< QString >           m_loadedTodos;
    QSet< QString >           m_newLoadedTodos;
    bool                      m_needUpdate;

    QPointer<TodoList> m_todoList;
    QPointer<Todo>     m_parentTodo;
    QString            m_filter;
    bool               m_showDone;
    bool               m_showDeleted;
    bool               m_hideUndeleted;
    QDateTime          m_maxDueDate;
    Todo::TodoSortMode m_sortMode;

    static QString idForTodo( const QString &backend, const QString &todoId );

private slots:

    void triggerUpdate();
    void addTodo( const QString &backend, const TodoStruct &todo );
    void removeExtraneousTodos();

    void handleTodoDeleted( QObject *todo );
};

/**
   @brief Query todos from the application storage
 */
class TodoStorageQuery : public TodoListStorageQuery
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

    bool hideUndeleted() const;
    void setHideUndeleted(bool hideUndeleted);

    // TodoListStorageQuery interface
    virtual void beginRun();
    virtual bool query(QString &query, QVariantMap &args);
    virtual void recordAvailable(const QVariantMap &record);
    virtual void endRun();


signals:

    void todoAvailable( const QString &backend, const TodoStruct &todo );
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

};

#endif // TODOMODEL_H
