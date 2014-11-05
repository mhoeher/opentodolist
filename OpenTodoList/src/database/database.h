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
   @brief Provides the todo list storage for the application

   This class is responsible to provide the todo list storage for the application. Internally,
   it will start an instance of the DatabaseWorker class to maintain a SQL database which is used
   for permanent object storage. The database is well hidden. The front end uses a high level
   API to add new object to the database, update existing ones or schedule objects for permanent
   deletion from the database.

   This class also is responsible to load the various backends (i.e. plugins implementing
   the IBackend interface). Backends are used for external todo data storage, i.e. they can
   bridge from the application to any external service that allows to store todo list information.
 */
class Database : public QObject
{
    Q_OBJECT
public:
    explicit Database(QObject *parent = 0);
    virtual ~Database();

    void runQuery( StorageQuery *query );
    void scheduleQuery( StorageQuery *query );

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

private slots:

    void startBackends();

};

} /* DataBase */

} /* OpenTodoList */

#endif // TODOLISTSTORAGE_H
