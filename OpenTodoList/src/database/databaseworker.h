#ifndef TODOLISTSTORAGEWORKER_H
#define TODOLISTSTORAGEWORKER_H

#include "core/opentodolistinterfaces.h"

#include <QMutex>
#include <QQueue>
#include <QSqlDatabase>
#include <QTemporaryFile>

namespace OpenTodoList {

namespace DataBase {

class StorageQuery;
class Database;

/**
   @brief Maintains the todo storage database

   The DatabaseWorker class is maintaining the SQL database used to store
   todo data in the application. The assumption is that the class is run in a dedicated
   database thread.

   The DatabaseWorker class is used internally by the Database class. It hence marks
   this one as a friend, so that the APIs can only be accessed by that class.
 */
class DatabaseWorker : public QObject
{
    Q_OBJECT

    friend class Database;

public:

    explicit DatabaseWorker();
    virtual ~DatabaseWorker();


    // Interface used by Database class:
private:
    void run( StorageQuery *query );
    void schedule( StorageQuery *query );

private slots:
    void init();

signals:

    void initialized();

    void backendChanged( const QVariant &backend );
    void accountChanged( const QVariant &account );
    void todoListChanged( const QVariant &todoList );
    void todoChanged( const QVariant &todo );
    void taskChanged( const QVariant &task );

    // Private area:
private:

    QSqlDatabase                    m_dataBase;
    QTemporaryFile                  m_dataBaseFile;
    bool                            m_initialized;
    QQueue< StorageQuery* >         m_queue;
    QMutex                          m_queueLock;
    QMutex                          m_runLock;

    void runSimpleQuery(const QString &query , const QString &errorMsg);
    void runQuery( StorageQuery *query );

    void updateToSchemaVersion0();

private slots:

    void next();
};

} /* DataBase */

} /* OpenTodoList */

#endif // TODOLISTSTORAGEWORKER_H
