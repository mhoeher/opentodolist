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

#ifndef DATASTORAGE_DELETEITEMSQUERY_H_
#define DATASTORAGE_DELETEITEMSQUERY_H_

#include <QObject>
#include <QUuid>

#include "datastorage/itemsquery.h"

class Item;
class Library;

class DeleteItemsQuery : public ItemsQuery
{
    Q_OBJECT
public:
    explicit DeleteItemsQuery(QObject *parent = nullptr);

    void deleteItem(const Item *item);
    void deleteItem(const QUuid &uid);
    void deleteLibrary(const Library *library, bool deleteLibraryDir);
    void deleteLibrary(const QUuid &uid, bool deleteLibraryDir);

signals:

    /**
     * @brief The item and all of its children have been deleted.
     *
     * This signal is emitted to indicate that the deletion of the item and
     * all of its children has finished.
     */
    void itemDeleted();

private:
    struct ItemToDelete
    {
        QUuid uid;
        bool isLibrary;
        bool deleteLibraryDir;
    };

    QList<ItemToDelete> m_itemsToDelete;

    // ItemsQuery interface
protected:
    void run() override;
};

#endif // DATASTORAGE_DELETEITEMSQUERY_H_
