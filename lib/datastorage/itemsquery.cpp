#include "itemsquery.h"

/**
 * @brief Constructor.
 */
ItemsQuery::ItemsQuery(QObject *parent) : QObject(parent),
    m_context(),
    m_global(),
    m_items(),
    m_children(),
    m_dataChanged(false)
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
 * @brief Indicate that the query changed the cache contents.
 *
 * This method shall be called by concrete sub-classes to indicate that the
 * cache contents have been changed.
 * @param changed
 */
void ItemsQuery::setDataChanged(bool changed)
{
    m_dataChanged = changed;
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
    if (m_dataChanged) {
        emit dataChanged();
    }
    emit finished();
}
