/*
 * Copyright 2020 Martin Hoeher <martin@rpdev.net>
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

#include "datamodel/library.h"
#include "datastorage/cache.h"
#include "datastorage/getlibraryitemsuidsitemsquery.h"
#include "datastorage/insertorupdateitemsquery.h"
#include "datastorage/libraryloader.h"
#include "deleteitemsquery.h"

/**
 * @brief Constructor.
 */
LibraryLoader::LibraryLoader(QObject *parent)
    : QObject(parent), m_libraryId(), m_directory(), m_cache(nullptr), m_scanWatcher()
{
    connect(&m_scanWatcher, &QFutureWatcher<DirectoryScanResult>::finished, this,
            &LibraryLoader::directoryScanDone);
}

/**
 * @brief Destructor.
 */
LibraryLoader::~LibraryLoader()
{
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
void LibraryLoader::setDirectory(const QString &directory)
{
    m_directory = directory;
}

/**
 * @brief The item cache the loader shall put items into.
 */
Cache *LibraryLoader::cache() const
{
    return m_cache;
}

/**
 * @brief Set he item cache.
 */
void LibraryLoader::setCache(Cache *cache)
{
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
void LibraryLoader::setLibraryId(const QUuid &libraryId)
{
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
    if (m_cache == nullptr || m_libraryId.isNull() || !QDir(m_directory).exists()) {
        QTimer::singleShot(0, this, &LibraryLoader::scanFinished);
    } else {
        auto q = new GetLibraryItemsUIDsItemsQuery();
        q->addLibrary(m_libraryId);
        connect(q, &GetLibraryItemsUIDsItemsQuery::uidsAvailable, this,
                &LibraryLoader::itemUidsLoaded, Qt::QueuedConnection);
        m_cache->run(q);
    }
}

void LibraryLoader::itemUidsLoaded(QSet<QUuid> uids)
{
    auto directory = m_directory;
    auto libraryId = m_libraryId;

    auto future = QtConcurrent::run([directory, libraryId, uids]() {
        DirectoryScanResult result;
        auto q = new InsertOrUpdateItemsQuery();
        Library lib(directory);
        if (lib.load()) {
            q->add(&lib);
        }
        result.query = q;
        result.itemsToDelete = uids;
        auto years = Library::years(directory);
        for (auto year : years) {
            auto months = Library::months(directory, year);
            for (auto month : months) {
                QDir dir(directory + "/" + year + "/" + month);
                QString suffix = "*." + Item::FileNameSuffix;
                for (auto entry : dir.entryList({ suffix }, QDir::Files)) {
                    auto item = Item::createItemFromFile(dir.absoluteFilePath(entry));
                    if (item) {
                        auto topLevelItem = qobject_cast<TopLevelItem *>(item);
                        if (topLevelItem != nullptr) {
                            topLevelItem->setLibraryId(libraryId);
                        }
                        q->add(item);
                        result.itemsToDelete.remove(item->uid());
                        delete item;
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
    auto result = m_scanWatcher.result();
    m_itemsToDelete = result.itemsToDelete;
    if (m_cache != nullptr) {
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
    if (m_itemsToDelete.isEmpty() || m_cache == nullptr) {
        emit scanFinished();
    } else {
        auto q = new DeleteItemsQuery();
        for (auto id : m_itemsToDelete) {
            q->deleteItem(id);
        }
        connect(q, &DeleteItemsQuery::finished, this, &LibraryLoader::scanFinished,
                Qt::QueuedConnection);
        m_cache->run(q);
    }
}
