/*
 * Copyright 2020-2021 Martin Hoeher <martin@rpdev.net>
 +
 * This file is part of OpenTodoList.
 *
 * OpenTodoList is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * OpenTodoList is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with OpenTodoList.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <QDir>
#include <QLoggingCategory>
#include <QRunnable>
#include <QScopedPointer>
#include <QStandardPaths>
#include <QTemporaryDir>
#include <QThreadPool>

#include <qlmdb/context.h>
#include <qlmdb/database.h>
#include <qlmdb/errors.h>
#include <qlmdb/transaction.h>

#include "cache.h"
#include "itemsquery.h"
#include "datamodel/item.h"
#include "datamodel/library.h"

static Q_LOGGING_CATEGORY(log, "OpenTodoList.Cache", QtWarningMsg);

const QByteArray Cache::RootId = "---root-item---";
const QByteArray Cache::VersionKey = "OpenTodoList.Cache.Version";
const QByteArray Cache::Version_0 = "0";
const QByteArray Cache::Version_1 = "1";

/**
 * @brief Runs an ItemQuery.
 */
class ItemsQueryRunnable : public QRunnable
{
public:
    explicit ItemsQueryRunnable(ItemsQuery* query);

    // QRunnable interface
    void run() override;

private:
    QScopedPointer<ItemsQuery> m_query;
};

/**
 * @brief Creates a runner which runs the @p query on the @p cache.
 */
ItemsQueryRunnable::ItemsQueryRunnable(ItemsQuery* query) : QRunnable(), m_query(query) {}

/**
 * @brief Implementation of QRunnable::run().
 */
void ItemsQueryRunnable::run()
{
    qCDebug(log) << "Running query" << m_query.data();
    m_query->run();
    m_query->finish();
    qCDebug(log) << "Query" << m_query.data() << "finished";
}

/**
 * @brief Constructor.
 */
Cache::Cache(QObject* parent)
    : QObject(parent),
      m_cacheDirectory(),
      m_cacheSize(
#if QT_POINTER_SIZE == 4
              1U * 1024U * 1024U * 1024U
#else
              3U * 1024U * 1024U * 1024U
#endif
              ),
      m_context(nullptr),
      m_global(),
      m_items(),
      m_children(),
      m_threadPool(new QThreadPool(this)),
      m_nonDBThreadPool(new QThreadPool(this)),
      m_numberOfRunningTransactions(0),
      m_valid(false)
{
}

/**
 * @brief Destructor.
 */
Cache::~Cache()
{
    // Explicitly delete the thread pool - this causes us to wait for remaining jobs. Otherwise,
    // we might have dangling jobs accessing already destroyed resources.
    delete m_nonDBThreadPool;
    m_nonDBThreadPool = nullptr;

    delete m_threadPool;
    m_threadPool = nullptr;
}

/**
 * @brief The directory where the on-disk cache is stored.
 */
QString Cache::cacheDirectory() const
{
    return m_cacheDirectory;
}

/**
 * @brief Set the directory where the on-disk cache is stored.
 */
void Cache::setCacheDirectory(const QString& cacheDirectory)
{
    m_cacheDirectory = cacheDirectory;
}

/**
 * @brief The maximum size of the cache.
 */
size_t Cache::cacheSize() const
{
    return m_cacheSize;
}

/**
 * @brief Set the maximum size of the cache to @p envSize.
 */
void Cache::setCacheSize(size_t envSize)
{
    m_cacheSize = envSize;
}

/**
 * @brief Open the cache.
 * @return
 */
bool Cache::open()
{
    m_valid = false;
    // Try to open env with initial size, but if we don't have enough disk space,
    // try to open it with less memory, down to at least 10MB.
    auto cacheSizeValue = m_cacheSize;
    do {
        m_context.reset(new QLMDB::Context);
        m_context->setMaxDBs(3);
        m_context->setPath(m_cacheDirectory);
        m_context->setMapSize(cacheSizeValue);
        if (m_context->open()) {
            break;
        } else if (m_context->lastError() == QLMDB::Errors::OutOfMemory) {
            m_context.clear();
            cacheSizeValue = cacheSizeValue / 2;
            if (cacheSizeValue < 10U * 1024U * 1024U) {
                qCWarning(log) << "Finally failed to open cache - we need at "
                                  "least 10MB.";
                break;
            } else {
                qCWarning(log) << "Failed to open cache with size of" << cacheSizeValue
                               << "bytes, trying smaller cache...";
            }
        } else {
            qCWarning(log) << "Failed to open cache directory" << m_context->path() << ":"
                           << m_context->lastErrorString();
            m_context.clear();
            break;
        }
    } while (m_context == nullptr);

    if (m_context != nullptr) {
        if (openDBs() && initVersion0() // cppcheck-suppress knownConditionTrueFalse
            && initVersion1()) { // cppcheck-suppress knownConditionTrueFalse
            // Clear the file timestamps cache - simply to avoid any issues across app
            // restarts.
            if (m_fileTimestamps->clear()) {
                m_valid = true;
            } else {
                qCWarning(log) << "Failed to clear file timestamp cache:"
                               << m_fileTimestamps->lastErrorString();
            }
        } else {
            qCWarning(log) << "Failed to initialize cache.";
            m_context.clear();
        }
    }
    return m_valid;
}

bool Cache::isValid() const
{
    return m_valid;
}

/**
 * @brief Run a query on the cache.
 *
 * This runs the @p query on this cache. The query is run in a dedicated
 * thread (using a QThreadPool). Hence, the query is run either immediately
 * or (if all threads are already busy) the query is enqueued and executed
 * as soon as a thread gets free.
 *
 * The ownership of the query object is passed to the cache, which will destroy
 * the query once it ran.
 */
void Cache::run(ItemsQuery* query)
{
    if (query != nullptr) {
        qCDebug(log) << "Running" << query << "on cache";
        if (m_context == nullptr) {
            qCWarning(log) << "Cache context is null - not running" << query;
            delete query;
        } else {
            qCDebug(log) << "Preparing query" << query << "for run";
            query->m_cache = this;
            query->m_context = m_context;
            query->m_global = m_global;
            query->m_items = m_items;
            query->m_children = m_children;
            connect(query, &ItemsQuery::dataChanged, this, &Cache::dataChanged,
                    Qt::QueuedConnection);
            connect(query, &ItemsQuery::librariesChanged, this, &Cache::librariesChanged,
                    Qt::QueuedConnection);
            connect(query, &ItemsQuery::finished, this, &Cache::finished, Qt::QueuedConnection);
            connect(
                    query, &ItemsQuery::finished, this,
                    [=]() { setNumberOfRunningTransactions(numberOfRunningTransactions() - 1); },
                    Qt::QueuedConnection);
            if (query->isNonDBQuery()) {
                m_nonDBThreadPool->start(new ItemsQueryRunnable(query));
            } else {
                m_threadPool->start(new ItemsQueryRunnable(query));
            }
            setNumberOfRunningTransactions(m_numberOfRunningTransactions + 1);
        }
    }
}

bool Cache::initialize(const QString& cacheDir)
{
    auto dir = cacheDir;
    if (dir.isEmpty()) {
        dir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    }
    dir += "/cache";
    {
        QDir d(dir);
        if (!d.exists()) {
            if (!d.mkpath(".")) {
                qWarning() << "Failed to create cache directory";
            }
        }
    }
    setCacheDirectory(dir);

    // WA for https://gitlab.com/rpdev/opentodolist/issues/214:
    // Try to use different directories for the cache - required if
    // the user previously ran the app with a different architecture (and
    // hence we cannot re-open the incompatible LMDB DB).
    {
        int i = 0;
        while (!open() && i < 10) {
            auto secondaryCacheDir = dir + "-" + QString::number(i++);
            QDir d(secondaryCacheDir);
            if (!d.exists()) {
                if (!d.mkpath(".")) {
                    qWarning() << "Failed to create secondary cache directory" << secondaryCacheDir;
                }
            }
            setCacheDirectory(secondaryCacheDir);
        }
    }

    if (!isValid()) {
        // If we still were not able to open a cache persistently,
        // resort to using a temporary directory. This means, we would
        // not cache between app restarts, but there should actually not
        // be a reason why we run into this situation (unless we have file
        // system corruptions or something like that).
        m_tmpCacheDir.reset(new QTemporaryDir);
        setCacheDirectory(m_tmpCacheDir->path());
        if (!open()) {
            qCWarning(log) << "Permanently failed to open a cache directory. "
                              "The app should not crash, but it likely won't "
                              "function as you might expect.";
        }
    }

    return isValid();
}

/**
 * @brief Save the timestamp for an item's file.
 *
 * This saves the timestamp of the data file the @p item is written to, using the @p transaction to
 * protect access to the cache.
 *
 * This returns true if file timestamp has been added to the cache, i.e. if it either has not
 * been written to the cache before or the file on disk changed and hence the item needs an update.
 *
 * This mechanism is used to detect if an item needs to be updated in the cache, i.e. before
 * actually writing to the cache, the item file's timestamp is written. Only if the update occurred,
 * we know that the item is more recent than what we currently have in the cache and hence we need
 * to update.
 */
bool Cache::setItemTimestamp(const Item* item, QLMDB::Transaction* transaction)
{
    Q_CHECK_PTR(transaction);
    if (item && item->isValid()) {
        QFileInfo fi(item->filename());
        if (fi.exists()) {
            auto lastModified = fi.lastModified().toString(Qt::ISODateWithMs).toUtf8();
            auto key = fi.absoluteFilePath().toUtf8();
            auto previousTimestamp = m_fileTimestamps->get(*transaction, key);
            if (previousTimestamp.isNull() || lastModified != previousTimestamp) {
                m_fileTimestamps->put(*transaction, key, lastModified);
                return true;
            }
        }
    }
    return false;
}

/**
 * @brief Save the timestamp for an item's file.
 *
 * This is an overloaded version of the setItemTimestamp() method, which does not take an
 * transaction object. Use this if you do not work with a transaction already - this method
 * will create one on the fly for you.
 */
bool Cache::setItemTimestamp(const Item* item)
{
    QLMDB::Transaction t(*m_context);
    return setItemTimestamp(item, &t);
}

/**
 * @brief Save the timestamp for a library's file.
 *
 * This is a method similar to setItemTimestamp() but for the @p library. The method will access the
 * cache using the specified @p transaction.
 */
bool Cache::setLibraryTimestamp(const Library* library, QLMDB::Transaction* transaction)
{
    Q_CHECK_PTR(transaction);
    if (library && library->isValid()) {
        QFileInfo fi(library->directory() + "/" + Library::LibraryFileName);
        if (fi.exists()) {
            auto lastModified = fi.lastModified().toString(Qt::ISODateWithMs).toUtf8();
            auto key = fi.absoluteFilePath().toUtf8();
            auto previousTimestamp = m_fileTimestamps->get(*transaction, key);
            if (previousTimestamp.isNull() || lastModified != previousTimestamp) {
                m_fileTimestamps->put(*transaction, key, lastModified);
                return true;
            }
        }
    }
    return false;
}

/**
 * @brief Save the timestamp for a library's file.
 *
 * This is an overloaded version which creates a transaction internally.
 */
bool Cache::setLibraryTimestamp(const Library* library)
{
    QLMDB::Transaction t(*m_context);
    return setLibraryTimestamp(library, &t);
}

/**
 * @brief Get the number of running transactions.
 *
 * This returns the number of jobs that are currently run on the cache.
 */
int Cache::numberOfRunningTransactions() const
{
    return m_numberOfRunningTransactions;
}

void Cache::setNumberOfRunningTransactions(int newNumberOfRunningTransactions)
{
    if (m_numberOfRunningTransactions == newNumberOfRunningTransactions)
        return;
    m_numberOfRunningTransactions = newNumberOfRunningTransactions;
    emit numberOfRunningTransactionsChanged();
}

bool Cache::openDBs()
{
    m_global = QSharedPointer<QLMDB::Database>(new QLMDB::Database(*m_context));
    m_items = QSharedPointer<QLMDB::Database>(new QLMDB::Database(*m_context, "items"));
    m_children = QSharedPointer<QLMDB::Database>(new QLMDB::Database(
            *m_context, "children", QLMDB::Database::Create | QLMDB::Database::MultiValues));
    m_fileTimestamps =
            QSharedPointer<QLMDB::Database>(new QLMDB::Database(*m_context, "fileTimestamps"));
    if (m_global->isValid()) {
        if (m_items->isValid()) {
            if (m_children->isValid()) {
                if (m_fileTimestamps->isValid()) {
                    m_valid = true;
                } else {
                    qCWarning(log) << "Failed to open file timestamps cache:"
                                   << m_fileTimestamps->lastErrorString();
                }
            } else {
                qCWarning(log) << "Failed to open children cache:" << m_children->lastErrorString();
            }
        } else {
            qCWarning(log) << "Failed to open item cache:" << m_children->lastErrorString();
        }
    } else {
        qCWarning(log) << "Failed to open global cache:" << m_global->lastErrorString();
    }
    return m_global->isValid() && m_items->isValid() && m_children->isValid();
}

bool Cache::initVersion0()
{
    auto version = m_global->get(VersionKey);
    if (version.isEmpty()) {
        qCDebug(log) << "Cache is uninitialized - initializing to Version 0 "
                        "now";
        m_global->put(VersionKey, Version_0);
    }
    return true;
}

bool Cache::initVersion1()
{
    auto version = m_global->get(VersionKey);
    if (version == Version_0) {
        qCDebug(log) << "Cache is at version 0 - need to drop data and update to version 1";
        QLMDB::Transaction t(*m_context);
        m_children->clear(t);
        m_items->clear(t);
        m_global->put(t, VersionKey, Version_1);
    }
    return true;
}
