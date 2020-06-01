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

#ifndef DATASTORAGE_INSERTORUPDATEITEMSQUERY_H_
#define DATASTORAGE_INSERTORUPDATEITEMSQUERY_H_

#include <QObject>
#include <QUuid>
#include <QSet>

#include "datamodel/item.h"
#include "datamodel/library.h"
#include "datastorage/itemsquery.h"

// Forward declaration:
class Item;

class InsertOrUpdateItemsQuery : public ItemsQuery
{
    Q_OBJECT
public:
    /**
     * @brief Run special instructions upon insertion/update.
     *
     * These flags can be used to run special instruction when the query is run.
     */
    enum InsertFlag {

        /**
         * @brief Just run the insertion/update.
         */
        NoFlags = 0,

        /**
         * @brief Calculate a new weight for the item.
         *
         * When this flag is set, a weight will be calculated for the item by
         * finding the maximum weight among all children of the same parent
         * item and adding some margin on top.
         *
         */
        CalcWeight = 0x01,

        /**
         * @brief Also save the item to disk.
         *
         * If this flag is set, also save the item to disk.
         */
        Save = 0x02,

        /**
         * @brief This is equivalent to `CalcWeight | Save`.
         */
        CreateNewItem = CalcWeight | Save
    };

    Q_DECLARE_FLAGS(InsertFlags, InsertFlag)

    explicit InsertOrUpdateItemsQuery(QObject* parent = nullptr);

public slots:

    void add(Item* item, InsertFlags flags = NoFlags);
    void add(Library* library, InsertFlags flags = NoFlags);

protected:
    // ItemsQuery interface
    void run() override;

private:
    QList<ItemCacheEntry> m_itemEntries;
    QList<LibraryCacheEntry> m_libEntries;
    QSet<QUuid> m_calcWeight;
    QSet<QUuid> m_save;
};

#endif // DATASTORAGE_INSERTORUPDATEITEMSQUERY_H_
