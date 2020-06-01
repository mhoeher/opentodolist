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

#ifndef DATASTORAGE_LIBRARYLOADER_H_
#define DATASTORAGE_LIBRARYLOADER_H_

#include <QObject>
#include <QThread>
#include <QFutureWatcher>

#include "datamodel/item.h"

class LibraryLoaderWorker;
class Cache;
class InsertOrUpdateItemsQuery;

class LibraryLoader : public QObject
{
    Q_OBJECT
public:
    explicit LibraryLoader(QObject* parent = nullptr);
    virtual ~LibraryLoader();

    QString directory() const;
    void setDirectory(const QString& directory);

    QUuid libraryId() const;
    void setLibraryId(const QUuid& libraryId);

    Cache* cache() const;
    void setCache(Cache* cache);

signals:

    /**
     * @brief Scanning has finished.
     */
    void scanFinished();

public slots:

    void scan();

private:
    struct DirectoryScanResult
    {
        InsertOrUpdateItemsQuery* query;
        QSet<QUuid> itemsToDelete;
    };

    QUuid m_libraryId;
    QString m_directory;
    Cache* m_cache;
    QFutureWatcher<DirectoryScanResult> m_scanWatcher;
    QSet<QUuid> m_itemsToDelete;

private slots:

    void itemUidsLoaded(QSet<QUuid> uids);
    void directoryScanDone();
    void itemsInserted();
};

#endif // DATASTORAGE_LIBRARYLOADER_H_
