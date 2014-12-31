#ifndef TODOLISTSTORAGEQUERY_H
#define TODOLISTSTORAGEQUERY_H

#include "core/opentodolistinterfaces.h"

#include <QObject>
#include <QQueue>
#include <QVariantMap>

namespace OpenTodoList {

namespace DataBase {

class DatabaseWorker;

/**
   @brief Base class for all database queries

   This class is used as the base for all database queries. In order to query or manipulate the
   database, one has to subclass this class and pass an instance of it to the OpenTodoList::Database
   class.
 */
class StorageQuery : public QObject
{
    Q_OBJECT
    friend class DatabaseWorker;
public:

    /**
       @brief Used to resource management

       This enum is used in some concrete storage queries to determine ownership of
       resources.
     */
    enum ResourceOwnership {
        QueryIsOwner, //!< The StorageQuery object takes over ownership of the resources and releases them upon destruction
        CallerIsOwner //!< The caller remains owner of any resources used by the query
    };

    explicit StorageQuery(QObject *parent = 0);
    virtual ~StorageQuery();

    virtual void beginRun();
    virtual bool query( QString &query, QVariantMap &args );
    virtual void recordAvailable( const QVariantMap &record );
    virtual void newIdAvailable( const QVariant &id );
    virtual void endRun();
    virtual bool hasNext() const;

    static ITodoList* todoListFromRecord( const QVariantMap &record );
    static ITodo* todoFromRecord( const QVariantMap &record );

signals:

    /**
       @brief This signal is emitted when the query is fully done

       This signal is emitted as soon as the query has been processed and will be deleted next
       by the scheduler.
     */
    void queryFinished();

    /**
       @brief A backend has been inserted or updated

       A query shall emit this signal to indicate that a backend has been updated in the
       database. This signal is later on broadcasted in the application, allowing registered
       clients to update their data on-the-fly.
     */
    void backendChanged( const QVariant &backend );

    /**
       @brief An account has been inserted or changed

       A query shall emit this signal to indicate that an account has been inserted
       or updated in the database. The signal will be broadcasted into the application,
       allowing registered clients to update on-the-fly.
     */
    void accountChanged( const QVariant &account );

    /**
       @brief A todo list has been inserted or updated

       A query shall emit this signal to indicate that a @p todoList has been added or updated in the
       database. The signal is broadcasted into the application, allowing clients to update
       on-the-fly.
     */
    void todoListChanged( const QVariant &todoList );

    /**
       @brief A todo has been inserted or changed

       A query shall emit this signal to indicate that a @p todo has been added or updated in the
       database. The signal is broadcasted into the application, allowing clients to update
       on-the-fly.
     */
    void todoChanged( const QVariant &todo );

    /**
       @brief A task has been inserted or updated

       A query shall emit this signal to indicate that a @p task has been
       added or updated in the database. The signal is broadcasted into
       the application, allowing clients to update on-the-fly.
     */
    void taskChanged( const QVariant &task );

    void accountDeleted( const QVariant &account );
    void todoListDeleted( const QVariant &todoList );
    void todoDeleted( const QVariant &todo );
    void taskDeleted( const QVariant &task );


public slots:

protected:

    DatabaseWorker *worker() const;

private:

    DatabaseWorker *m_worker;

};

} /* DataBase */

} /* OpenTodoList */

#endif // TODOLISTSTORAGEQUERY_H
