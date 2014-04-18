#include "todoliststoragequery.h"

/**
   @brief Constructor
 */
TodoListStorageQuery::TodoListStorageQuery(QObject *parent) :
    QObject(parent)
{
}

/**
   @brief Destructor
 */
TodoListStorageQuery::~TodoListStorageQuery()
{

}

/**
   @brief The query is to be executed

   This method is called when the query is to be executed.
 */
void TodoListStorageQuery::beginRun()
{
    // nothing to be done here
}

/**
   @brief The query to be run

   This method is called to get the query that is to be run. Sub-classes
   should write the SQL query into @p query. If the query contains any
   user specified values, they can be put into the @p args map. When executing
   the query, the args will be inserted into the query string in a safe way.

   @return True in case everything is okay. In that case, the query will next
     be run on the database. If this returns false, nothing happens and
     execution will stop (but endRun() will still be called.
 */
bool TodoListStorageQuery::query(QString &query, QVariantMap &args)
{
    Q_UNUSED( query );
    Q_UNUSED( args );
    return false;
}

/**
   @brief A new record is available

   This is called for each record that is read from the database when
   running the query.

   The values read are passed in via the @p record map.
 */
void TodoListStorageQuery::recordAvailable(const QVariantMap &record)
{
    Q_UNUSED( record );
}

/**
   @brief Running the query has finished

   This method is called when running the query is done.
 */
void TodoListStorageQuery::endRun()
{

}
