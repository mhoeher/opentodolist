#ifndef TODOLISTSTORAGEQUERY_H
#define TODOLISTSTORAGEQUERY_H

#include "opentodolistinterfaces.h"

#include <QObject>
#include <QQueue>
#include <QVariantMap>

class TodoListStorageWorker;

class TodoListStorageQuery : public QObject
{
    Q_OBJECT
    friend class TodoListStorageWorker;
public:
    explicit TodoListStorageQuery(QObject *parent = 0);
    virtual ~TodoListStorageQuery();

    virtual void beginRun();
    virtual bool query( QString &query, QVariantMap &args );
    virtual void recordAvailable( const QVariantMap &record );
    virtual void endRun();
    virtual bool hasNext() const;

    static TodoListStruct todoListFromRecord( const QVariantMap &record );
    static TodoStruct todoFromRecord( const QVariantMap &record );

signals:

public slots:

protected:

    TodoListStorageWorker *worker() const;

private:

    TodoListStorageWorker *m_worker;

};

/**
   @brief A simple query for getting todo lists by their ID
 */
class TodoListByIdQuery : public TodoListStorageQuery
{
    Q_OBJECT
public:
    explicit TodoListByIdQuery( const QString &backend, const QString &todoListId, QObject* parent = 0 );
    virtual ~TodoListByIdQuery();

    virtual bool query( QString &query, QVariantMap &args );
    virtual void recordAvailable( const QVariantMap &record );

signals:

    void todoListAvailable( const QString &backend, const TodoListStruct &list );

private:

    QString m_backend;
    QString m_todoListId;
};

class RecursiveDeleteTodoListQuery : public TodoListStorageQuery
{
    Q_OBJECT
public:
    explicit RecursiveDeleteTodoListQuery( const QString &backend, const TodoListStruct &list, QObject *parent = 0 );

    virtual void beginRun();
    virtual bool query(QString &query, QVariantMap &args );
    virtual void recordAvailable(const QVariantMap &record );
    virtual void endRun();

signals:
    void notifyTodoListDeleted( const QString &backend, const TodoListStruct list );
    void notifyTodoDeleted( const QString &backend, const TodoStruct todo );

private:

    QString m_backend;
    TodoListStruct m_todoList;
    QQueue< TodoStruct > m_todosToDelete;
};

class RecursiveDeleteTodoQuery : public TodoListStorageQuery
{
    Q_OBJECT
public:
    explicit RecursiveDeleteTodoQuery( const QString &backend, const TodoStruct &todo, QObject *parent = 0 );

    virtual void beginRun();
    virtual bool query( QString &query, QVariantMap &args );
    virtual void recordAvailable( const QVariantMap &record );
    virtual bool hasNext() const;
    virtual void endRun();

signals:

    void notifyTodoDeleted( const QString &backend, const TodoStruct &todo );

private:
    QString m_backend;
    TodoStruct m_currentTodo;
    QQueue<TodoStruct> m_todosToDelete;
};

#endif // TODOLISTSTORAGEQUERY_H
