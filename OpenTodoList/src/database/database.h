#ifndef TODOLISTSTORAGE_H
#define TODOLISTSTORAGE_H

#include "core/opentodolistinterfaces.h"
#include "database/backendwrapper.h"
#include "pluginsloader.h"

#include <QObject>
#include <QQueue>
#include <QThread>

namespace OpenTodoList {

namespace DataBase {

class StorageQuery;
class DatabaseWorker;

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
class Database : public QObject
{
    Q_OBJECT
public:
    explicit Database(QObject *parent = 0);
    virtual ~Database();

    void runQuery( StorageQuery *query );

signals:

    void todoListInserted( const QString &backend, const ITodoList *list );
    void todoListRemoved( const QString &backend, const ITodoList *list );
    void todoInserted( const QString &backend, const ITodo *todo );
    void todoRemoved( const QString &backend, const ITodo *todo );

    void startBackends();
    void stopBackends();

public slots:

    bool insertTodoList( const QString &backend, const ITodoList *list );
    bool insertTodo( const QString &backend, const ITodo *todo );
    bool deleteTodoList( const QString &backend, const ITodoList *list );
    bool deleteTodo( const QString &backend, const ITodo *todo );

    void start();
    void stop();

private:

    QThread                          m_workerThread;
    DatabaseWorker                  *m_worker;
    PluginsLoader<IBackend>         *m_backendPlugins;

    QThread                          m_backendsThread;
    QVector< BackendWrapper* >       m_backends;

    BackendWrapper* backendByName( const QString &backend ) const;

#ifdef Q_OS_ANDROID
    static QString androidExtStorageLocation();
#endif
    static QString localStorageLocation( const QString &type );

};

} /* DataBase */

} /* OpenTodoList */

#endif // TODOLISTSTORAGE_H
