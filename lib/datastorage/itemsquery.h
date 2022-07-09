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

#ifndef DATASTORAGE_ITEMSQUERY_H_
#define DATASTORAGE_ITEMSQUERY_H_

#include <QDateTime>
#include <QHash>
#include <QObject>
#include <QSet>
#include <QSharedPointer>
#include <QVariant>

class Cache;
class Item;
class ItemCacheEntry;
class ItemsQueryRunnable;

// Forward declarations:
namespace QLMDB {
class Context;
class Database;
class Transaction;
}

class ItemsQuery : public QObject
{
    Q_OBJECT
    friend class ItemsQueryRunnable;
    friend class Cache;

public:
    explicit ItemsQuery(QObject* parent = nullptr);
    ~ItemsQuery() override;

signals:

    /**
     * @brief This signal is emitted to indicate that the query is done.
     */
    void finished();

    /**
     * @brief Indicates that he query changed the cache contents.
     *
     * This signal is emitted once when the query is finished and if
     * the query changed the cache contents (by calling setDataChanged()).
     */
    void dataChanged();

    /**
     * @brief A list of UIDs of libraries which have been changed.
     *
     * This signal is emitted by some queries if they changed the database.
     * It contains a list of UIDs of the libraries which have been changed.
     */
    void librariesChanged(QVariantList libraries);

protected:
    QSharedPointer<QLMDB::Context> context() const;
    QSharedPointer<QLMDB::Database> global() const;
    QSharedPointer<QLMDB::Database> items() const;
    QSharedPointer<QLMDB::Database> children() const;
    void setDataChanged(bool changed = true);
    bool hasDataChanged() const;

    struct PercentageForListOfItems
    {
        int numSubtasks;
        int numDoneSubtasks;
        int percentageDone;
    };

    /**
     * @brief Run the query.
     *
     * Sub-classes must implement this class to provide actual functionality.
     * Within their implementation, they can use methods like global(),
     * items() and children() to get the cache databases.
     */
    virtual void run() = 0;

    virtual void markAsChanged(QLMDB::Transaction* transaction, QByteArray id);

    double weightForNextItem(const QByteArray& parentId, QLMDB::Transaction& transaction);

    void calculateValues(QLMDB::Transaction& transaction, ItemCacheEntry* entry,
                         Item* item = nullptr);
    PercentageForListOfItems percentageForListOfItems(QLMDB::Transaction& transaction,
                                                      const QByteArray& todoId);
    QDateTime earliestChildDueDate(QLMDB::Transaction& transaction, const QByteArray& parentId);
    QDateTime earliestChildUpdatedAt(QLMDB::Transaction& transaction, const QByteArray& parentId);

    QSharedPointer<Item> itemFromCache(QLMDB::Transaction& t, const QUuid& itemUid);
    QVector<QUuid> lookupParents(QLMDB::Transaction& t, const Item* item);

private:
    QSharedPointer<QLMDB::Context> m_context;
    QSharedPointer<QLMDB::Database> m_global;
    QSharedPointer<QLMDB::Database> m_items;
    QSharedPointer<QLMDB::Database> m_children;
    bool m_dataChanged;
    QSet<QByteArray> m_changedLibrariesUids;
    QSet<QByteArray> m_changedParentUids;
    QHash<QUuid, QUuid> m_parentsMap; // Maps from item UID -> parent UID

    void finish();
};

#endif // DATASTORAGE_ITEMSQUERY_H_
