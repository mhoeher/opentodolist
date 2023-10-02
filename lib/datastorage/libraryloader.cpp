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

#include <QDebug>
#include <QTimer>
#include <QVariantMap>
#include <QtConcurrent>
#include <QLoggingCategory>

#include "datamodel/library.h"
#include "datastorage/cache.h"
#include "datastorage/getlibraryitemsuidsitemsquery.h"
#include "datastorage/insertorupdateitemsquery.h"
#include "datastorage/libraryloader.h"
#include "deleteitemsquery.h"

static Q_LOGGING_CATEGORY(log, "OpenTodoList.LibraryLoader", QtWarningMsg);

/**
 * @brief Constructor.
 */
LibraryLoader::LibraryLoader(QObject* parent)
    : QObject(parent), m_libraryId(), m_directory(), m_cache(nullptr), m_scanWatcher()
{
    qCDebug(log) << "Created library loader" << this;
    connect(&m_scanWatcher, &QFutureWatcher<DirectoryScanResult>::finished, this,
            &LibraryLoader::directoryScanDone);
}

/**
 * @brief Destructor.
 */
LibraryLoader::~LibraryLoader()
{
    qCDebug(log) << "Deleting library loader" << this;
    if (m_scanWatcher.isRunning()) {
        m_scanWatcher.waitForFinished();
    }
}

/**
 * @brief The directory to scan for items.
 */
QString LibraryLoader::directory() const
{
    return m_directory;
}

/**
 * @brief Set the directory to scan for items.
 */
void LibraryLoader::setDirectory(const QString& directory)
{
    qCDebug(log) << "Setting directory of" << this << "to" << directory;
    m_directory = directory;
}

/**
 * @brief The item cache the loader shall put items into.
 */
Cache* LibraryLoader::cache() const
{
    return m_cache;
}

/**
 * @brief Set he item cache.
 */
void LibraryLoader::setCache(Cache* cache)
{
    qCDebug(log) << "Setting cache of" << this << "to" << cache;
    m_cache = cache;
}

/**
 * @brief The UID of the library to load.
 */
QUuid LibraryLoader::libraryId() const
{
    return m_libraryId;
}

/**
 * @brief Set the UID of the library to load items for.
 */
void LibraryLoader::setLibraryId(const QUuid& libraryId)
{
    qCDebug(log) << "Setting library ID of" << this << "to" << libraryId;
    m_libraryId = libraryId;
}

/**
 * @brief Scan the directory for items.
 *
 * This triggers a scan of the directory for any kind of known item.
 * For each item discovered, the @p itemLoaded() signal is emitted.
 */
void LibraryLoader::scan()
{
    qCDebug(log) << "Start scan of" << this;
    if (m_cache == nullptr || m_libraryId.isNull() || !QDir(m_directory).exists()) {
        qCWarning(log) << "Library loader" << this << "is incomplete - not scanning";
        QTimer::singleShot(0, this, &LibraryLoader::scanFinished);
    } else {
        qCDebug(log) << "Requesting library UIDs in" << this;
        auto q = new GetLibraryItemsUIDsItemsQuery();
        q->addLibrary(m_libraryId);
        connect(q, &GetLibraryItemsUIDsItemsQuery::uidsAvailable, this,
                &LibraryLoader::itemUidsLoaded, Qt::QueuedConnection);
        m_cache->run(q);
    }
}

void LibraryLoader::itemUidsLoaded(QSet<QUuid> uids)
{
    qCDebug(log) << "UIDs loaded for" << this;

    auto directory = m_directory;
    auto libraryId = m_libraryId;
    auto cache = m_cache;

    auto future = QtConcurrent::run([cache, directory, libraryId, uids]() {
        qCDebug(log) << "Loading items from" << directory;
        DirectoryScanResult result;
        auto q = new InsertOrUpdateItemsQuery();
        qCDebug(log) << "Loading library meta data from" << directory;
        Library lib(directory);
        if (lib.load()) {
            qCDebug(log) << "Library meta data successfully loaded from" << directory;
            if (cache->setLibraryTimestamp(&lib)) {
                q->add(&lib);
            }
        } else {
            qCWarning(log) << "Failed to load library meta data from" << directory;
        }
        result.query = q;
        result.itemsToDelete = uids;
        const auto years = Library::years(directory);
        for (const auto& year : years) {
            const auto months = Library::months(directory, year);
            for (const auto& month : months) {
                QDir dir(directory + "/" + year + "/" + month);
                qCDebug(log) << "Checking directory" << dir;
                const auto itemFiles = Library::itemFiles(directory, year, month);
                for (const auto& entry : itemFiles) {
                    qCDebug(log) << "Checking file" << entry << "in" << dir;
                    auto item = Item::createItemFromFile(dir.absoluteFilePath(entry));
                    if (item) {
                        qCDebug(log) << "Found valid item" << item;
                        auto topLevelItem = qobject_cast<TopLevelItem*>(item);
                        if (topLevelItem != nullptr) {
                            topLevelItem->setLibraryId(libraryId);
                        }
                        if (cache->setItemTimestamp(item)) {
                            q->add(item);
                        }
                        result.itemsToDelete.remove(item->uid());
                        delete item;
                    } else {
                        qCWarning(log) << "Ignoring invalid item file" << entry << "in" << dir;
                    }
                }
            }
        }
        return result;
    });
    m_scanWatcher.setFuture(future);
}

void LibraryLoader::directoryScanDone()
{
    qCDebug(log) << "Directory scan of" << this << "finished";
    auto result = m_scanWatcher.result();
    m_itemsToDelete = result.itemsToDelete;
    if (m_cache != nullptr) {
        qCDebug(log) << "Inserted loaded items into cache for" << this;
        connect(result.query, &InsertOrUpdateItemsQuery::finished, this,
                &LibraryLoader::itemsInserted, Qt::QueuedConnection);
        m_cache->run(result.query);
    } else {
        delete result.query;
        emit scanFinished();
    }
}

void LibraryLoader::itemsInserted()
{
    qCDebug(log) << "Items inserted for" << this;
    if (m_itemsToDelete.isEmpty() || m_cache == nullptr) {
        qCDebug(log) << "Scan of" << this << "finished";
        emit scanFinished();
    } else {
        qCDebug(log) << "Removing deleted items from cache for" << this;
        auto q = new DeleteItemsQuery();
        const auto& itemsToDelete = m_itemsToDelete;
        for (auto id : itemsToDelete) {
            q->deleteItem(id);
        }
        connect(q, &DeleteItemsQuery::finished, this, &LibraryLoader::scanFinished,
                Qt::QueuedConnection);
        m_cache->run(q);
    }
}
