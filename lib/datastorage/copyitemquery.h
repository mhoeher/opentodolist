/*
 * Copyright 2021 Martin Hoeher <martin@rpdev.net>
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

#ifndef DATASTORAGE_COPYITEMQUERY_H_
#define DATASTORAGE_COPYITEMQUERY_H_

#include <QDir>
#include <QObject>
#include <QUuid>

#include "datastorage/itemsquery.h"

class Library;
class Item;

class CopyItemQuery : public ItemsQuery
{
    Q_OBJECT
public:
    explicit CopyItemQuery(QObject* parent = nullptr);

    void copyItem(Item* item, Library* targetLibrary, Item* parentItem = nullptr);

protected:
    // ItemsQuery interface
    void run() override;

private:
    QUuid m_sourceItemUid;
    QUuid m_targetLibraryUid;
    QDir m_targetPath;
    QUuid m_targetParentItemUid;
};

#endif // DATASTORAGE_COPYITEMQUERY_H_
