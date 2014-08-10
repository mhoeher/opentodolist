#ifndef TODOLISTSTORAGE_H
#define TODOLISTSTORAGE_H

#include "opentodolistinterfaces.h"

#include <QMutex>
#include <QObject>
#include <QQueue>
#include <QSqlDatabase>
#include <QTemporaryFile>
#include <QThread>

class TodoListStorageQuery;
class TodoListStorageWorker;

/**
   @brief The storage used to hold all user objects in the todo list application

   This class is used to encapsulate the internally used storage database and
   provide thread-safe access to it.

   Two types of APIs are provided:

   The high level API provides a slot based approach for inserting/updating
   and removing objects.

   The lower level query API allows to run more complex queries.

   Boths APIs are run in a separate thread, so the GUI thread won't block even
   when more complex processing needs to be done.
 */
class TodoListStorage : public QObject
{
    Q_OBJECT
public:
    explicit TodoListStorage(QObject *parent = 0);
    virtual ~TodoListStorage();

    void runQuery( TodoListStorageQuery *query );

signals:

    void todoListInserted( const QString &backend, const TodoListStruct &list );
    void todoListRemoved( const QString &backend, const TodoListStruct &list );
    void todoInserted( const QString &backend, const TodoStruct &todo );
    void todoRemoved( const QString &backend, const TodoStruct &todo );

public slots:

    bool insertTodoList( const QString &backend, const TodoListStruct &list );
    bool insertTodo( const QString &backend, const TodoStruct &todo );
    bool deleteTodoList( const QString &backend, const TodoListStruct &list );
    bool deleteTodo( const QString &backend, const TodoStruct &todo );

private:

    QThread                         m_thread;
    TodoListStorageWorker          *m_worker;

};

/**
   @brief Executes tasks of the storage in a backgroung thread
 */
class TodoListStorageWorker : public QObject
{
    Q_OBJECT

public:

    explicit TodoListStorageWorker();
    virtual ~TodoListStorageWorker();

    void run( TodoListStorageQuery *query );

public slots:

    void init();
    void insertTodoList( const QString &backend, const TodoListStruct &list );
    void insertTodo( const QString &backend, const TodoStruct &todo );
    void deleteTodoList( const QString &backend, const TodoListStruct &list, bool update = false );
    void deleteTodo( const QString &backend, const TodoStruct &todo,  bool update = false );

signals:

    void todoListInserted( const QString &backend, const TodoListStruct &list );
    void todoListRemoved( const QString &backend, const TodoListStruct &list );
    void todoInserted( const QString &backend, const TodoStruct &todo );
    void todoRemoved( const QString &backend, const TodoStruct &todo );

private:

    QSqlDatabase                    m_dataBase;
    QTemporaryFile                  m_dataBaseFile;
    bool                            m_initialized;
    QQueue< TodoListStorageQuery* > m_queue;
    QMutex                          m_queueLock;

    void insertRow( const QString &tableName, const QVariantMap &row );
    void deleteRow( const QString &tableName, const QVariantMap &row );

private slots:

    void next();
};

#endif // TODOLISTSTORAGE_H
