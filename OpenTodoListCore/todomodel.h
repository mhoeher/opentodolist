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
    Q_PROPERTY( int count READ rowCount NOTIFY countChanged )

public:

    /**
       @brief The type of listing the model shows
     */
    enum QueryType {
        InvalidQuery, //!< The model shows nothing
        QueryTopLevelTodosInTodoList, //!< The model shows top level todos in a todo list
        QuerySubTodosOfTodo //!< The model shows sub-todos of a given todo
    };

    explicit TodoModel(QObject *parent = 0);
    virtual ~TodoModel();

    TodoListLibrary *library() const;
    void setLibrary( TodoListLibrary *library );

    // QAbstractItemModel interface
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
    virtual QVariant data(const QModelIndex &index, int role) const;

    QueryType queryType() const;
    void setQueryType(const QueryType &queryType);

    TodoList *todoList() const;
    void setTodoList(TodoList *todoList);

    Todo *parentTodo() const;
    void setParentTodo(Todo *parentTodo);

signals:

    void libraryChanged();
    void queryTypeChanged();
    void todoListChanged();
    void parentTodoChanged();
    void changed();
    void countChanged();

public slots:

    void update();

private:

    QueryType                 m_queryType;
    QVector< Todo* >          m_todos;
    QPointer<TodoListLibrary> m_library;
    QSet< QString >           m_loadedTodos;
    QSet< QString >           m_newLoadedTodos;
    bool                      m_needUpdate;

    QPointer<TodoList> m_todoList;
    QPointer<Todo>     m_parentTodo;

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

};

#endif // TODOMODEL_H
