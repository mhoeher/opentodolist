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

#ifndef DATASTORAGE_CACHE_H_
#define DATASTORAGE_CACHE_H_

#include <QObject>
#include <QSharedPointer>
#include <QVariant>

// Forward declarations:
namespace QLMDB {
class Context;
class Database;
class Transaction;
}

class QThreadPool;
class ItemsQuery;
class QTemporaryDir;

class Item;
class Library;

/**
 * @brief On-disk items cache.
 *
 * This class provides an on-disk cache for item data (excluding larger binary
 * data, like attached files). The advantage of having such a persistent cache
 * is fast startup time: The app can start showing the user the last known state
 * while in the background an update is running.
 *
 * This class provides a very easy to use interface to write items to and read
 * them from the cache.
 */
class Cache : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int numberOfRunningTransactions READ numberOfRunningTransactions WRITE
                       setNumberOfRunningTransactions NOTIFY numberOfRunningTransactionsChanged)
public:
    static const QByteArray RootId;
    static const QByteArray VersionKey;
    static const QByteArray Version_0;
    static const QByteArray Version_1;

    explicit Cache(QObject* parent = nullptr);
    virtual ~Cache();

    QString cacheDirectory() const;
    void setCacheDirectory(const QString& cacheDirectory);

    size_t cacheSize() const;
    void setCacheSize(size_t envSize);

    bool open();
    bool isValid() const;

    void run(ItemsQuery* query);

    bool initialize(const QString& cacheDir = QString());

    bool setItemTimestamp(const Item* item, QLMDB::Transaction* transaction);
    bool setItemTimestamp(const Item* item);
    bool setLibraryTimestamp(const Library* library, QLMDB::Transaction* transaction);
    bool setLibraryTimestamp(const Library* library);

    int numberOfRunningTransactions() const;

signals:

    /**
     * @brief Data in the cache has changed.
     *
     * This signal is emitted to indicate that data in the cache has been
     * changed. Listeners may wait for this signal to learn when to re-read
     * item lists after a change.
     */
    void dataChanged();

    /**
     * @brief One or more libraries have changed.
     *
     * This signal is emitted if a query has finished running which modified
     * one or more libraries. The @p libraryUids parameter is a list of UIDs of
     * the libraries which have changed.
     */
    void librariesChanged(QVariantList libraryUids);

    /**
     * @brief A query has finished running.
     *
     * This signal is emitted to indicate that a query finished running.
     */
    void finished();

    /**
     * @brief The number of jobs running on the cache changed.
     */
    void numberOfRunningTransactionsChanged();

public slots:

private:
    QString m_cacheDirectory;
    size_t m_cacheSize;

    QSharedPointer<QLMDB::Context> m_context;
    QSharedPointer<QLMDB::Database> m_global;
    QSharedPointer<QLMDB::Database> m_items;
    QSharedPointer<QLMDB::Database> m_children;
    QSharedPointer<QLMDB::Database> m_fileTimestamps;
    QSharedPointer<QTemporaryDir> m_tmpCacheDir;
    QThreadPool* m_threadPool;
    QThreadPool* m_nonDBThreadPool;
    int m_numberOfRunningTransactions;
    bool m_valid;

    bool openDBs();
    bool initVersion0();
    bool initVersion1();

    void setNumberOfRunningTransactions(int newNumberOfRunningTransactions);
};

#endif // DATASTORAGE_CACHE_H_
