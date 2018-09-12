#include "itemsquery.h"

/**
 * @brief Constructor.
 */
ItemsQuery::ItemsQuery(QObject *parent) : QObject(parent)
{
}


/**
 * @brief Destructor.
 */
ItemsQuery::~ItemsQuery()
{
}


/**
 * @brief The database containing global settings.
 */
QSharedPointer<QLMDB::Database> ItemsQuery::global() const
{
    return m_global;
}


/**
 * @brief The database containing item data.
 */
QSharedPointer<QLMDB::Database> ItemsQuery::items() const
{
    return m_items;
}


/**
 * @brief The database containing parent/child relationships.
 */
QSharedPointer<QLMDB::Database> ItemsQuery::children() const
{
    return m_children;
}


/**
 * @brief The context of the cache data base.
 */
QSharedPointer<QLMDB::Context> ItemsQuery::context() const
{
    return m_context;
}


/**
 * @brief Indicate that the query is done.
 *
 * This method shall be called by sub-classes of the ItemsQuery class.
 * It will cause the finished() signal to be emitted, which other classes
 * should use to get informed that the query has been processed.
 */
void ItemsQuery::finish()
{
    emit finished();
}
