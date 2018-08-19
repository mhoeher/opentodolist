#include <QLoggingCategory>

#include <qlmdb/context.h>
#include <qlmdb/database.h>
#include <qlmdb/errors.h>

#include "cache.h"

static Q_LOGGING_CATEGORY(log, "OpenTodoList.Cache", QtWarningMsg)


const QByteArray Cache::RootId = "---root-item---";
const QByteArray Cache::VersionKey = "OpenTodoList.Cache.Version";
const QByteArray Cache::Version_0 = "0";


/**
 * @brief Constructor.
 */
Cache::Cache(QObject *parent) : QObject(parent),
    m_context(new QLMDB::Context),
    m_global(),
    m_items(),
    m_children(),
    m_valid(false)
{
    m_context->setMaxDBs(3);
}


/**
 * @brief Destructor.
 */
Cache::~Cache()
{
}


/**
 * @brief The directory where the on-disk cache is stored.
 */
QString Cache::cacheDirectory() const
{
    return m_context->path();
}


/**
 * @brief Set the directory where the on-disk cache is stored.
 */
void Cache::setCacheDirectory(const QString &cacheDirectory)
{
    m_context->setPath(cacheDirectory);
}


/**
 * @brief The maximum size of the cache.
 */
size_t Cache::cacheSize() const
{
    return m_context->mapSize();
}


/**
 * @brief Set the maximum size of the cache to @p envSize.
 */
void Cache::setCacheSize(size_t envSize)
{
    m_context->setMapSize(envSize);
}


/**
 * @brief Open the cache.
 * @return
 */
bool Cache::open()
{
    // Set max map size to 3GB
    m_context->setMapSize(static_cast<size_t>(3U * 1024U * 1024U * 1024U));

    // Try to open env with 3GB, but if we don't have enough disk space,
    // try to open it with less memory, down to at least 10MB.
    while (!m_context->open() &&
           m_context->lastError() == QLMDB::Errors::OutOfMemory) {
        m_context->setMapSize(m_context->mapSize() / 2);
        if (m_context->mapSize() < 10U * 1024U * 1024U) {
            qCWarning(log) << "Finally failed to open cache - we need at "
                              "least 10MB.";
            return false;
        }
    }

    if (m_context->isOpen() && openDBs() && initV0()) {
        m_valid = true;
    } else {
        qCWarning(log) << "Failed to open cache directory"
                       << m_context->path() << ":"
                       << m_context->lastErrorString();
    }
    return m_valid;
}

bool Cache::isValid() const
{
    return m_valid;
}

bool Cache::openDBs()
{
    m_global = QSharedPointer<QLMDB::Database>(
                new QLMDB::Database(*m_context.data()));
    m_items = QSharedPointer<QLMDB::Database>(
                new QLMDB::Database(*m_context.data(), "items"));
    m_children = QSharedPointer<QLMDB::Database>(
                new QLMDB::Database(*m_context.data(), "children",
                                    QLMDB::Database::Create |
                                    QLMDB::Database::MultiValues));
    if (m_global->isValid()) {
        if (m_items->isValid()) {
            if (m_children->isValid()) {
                m_valid = true;
            } else {
                qCWarning(log) << "Failed to open children cache:"
                               << m_children->lastErrorString();
            }
        } else {
            qCWarning(log) << "Failed to open item cache:"
                           << m_children->lastErrorString();
        }
    } else {
        qCWarning(log) << "Failed to open global cache:"
                       << m_global->lastErrorString();
    }
    return m_global->isValid() && m_items->isValid() &&
            m_children->isValid();
}

bool Cache::initV0()
{
    auto version = m_global->get(VersionKey);
    if (version.isEmpty()) {
        qCDebug(log) << "Cache is uninitialized - initializing to Version 0 "
                        "now";
        m_global->put(VersionKey, Version_0);
    }
    return true;
}
