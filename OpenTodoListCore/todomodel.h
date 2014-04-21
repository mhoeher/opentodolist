#ifndef TODOMODEL_H
#define TODOMODEL_H

#include "opentodolistinterfaces.h"
#include "todo.h"
#include "todoliststoragequery.h"

#include <QAbstractListModel>
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
    Q_PROPERTY( TodoListLibrary* library READ library WRITE setLibrary NOTIFY libraryChanged )
    Q_PROPERTY( QString backend READ backend WRITE setBackend NOTIFY backendChanged)
    Q_PROPERTY( QString todoListId READ todoListId WRITE setTodoListId NOTIFY todoListIdChanged)
    Q_PROPERTY( QString parentTodoId READ parentTodoId WRITE setParentTodoId NOTIFY parentTodoIdChanged)

public:
    explicit TodoModel(QObject *parent = 0);
    virtual ~TodoModel();

    TodoListLibrary *library() const;
    void setLibrary( TodoListLibrary *library );

    // QAbstractItemModel interface
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
    virtual QVariant data(const QModelIndex &index, int role) const;

    QString backend() const;
    void setBackend(const QString &backend);

    QString todoListId() const;
    void setTodoListId(const QString &todoListId);

    QString parentTodoId() const;
    void setParentTodoId(const QString &parentTodoId);

signals:

    void libraryChanged();
    void backendChanged();
    void todoListIdChanged();
    void parentTodoIdChanged();
    void changed();

public slots:

    void update();

private:

    QVector< Todo* > m_todos;
    TodoListLibrary *m_library;
    QSet< QString >  m_loadedTodos;
    QSet< QString >  m_newLoadedTodos;

    QString m_backend;
    QString m_todoListId;
    QString m_parentTodoId;

    static QString idForTodo( const QString &backend, const QString &todoId );

private slots:

    void addTodo( const QString &backend, const TodoStruct &todo );
    void removeExtraneousTodos();


};

/**
   @brief Query todos from the application storage
 */
class TodoStorageQuery : public TodoListStorageQuery
{
    Q_OBJECT
public:

    QString backend() const;
    void setBackend(const QString &backend);

    QString todoListId() const;
    void setTodoListId(const QString &todoListId);

    QString parentTodoId() const;
    void setParentTodoId(const QString &parentTodoId);

    // TodoListStorageQuery interface
    virtual void beginRun();
    virtual bool query(QString &query, QVariantMap &args);
    virtual void recordAvailable(const QVariantMap &record);
    virtual void endRun();

signals:

    void todoAvailable( const QString &backend, const TodoStruct &todo );
    void finished();

private:

    QString m_backend;
    QString m_todoListId;
    QString m_parentTodoId;

};

#endif // TODOMODEL_H
