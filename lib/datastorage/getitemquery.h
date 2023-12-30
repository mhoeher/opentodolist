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

#ifndef DATASTORAGE_GETITEMQUERY_H_
#define DATASTORAGE_GETITEMQUERY_H_

#include <QObject>
#include <QUuid>

#include "datastorage/itemsquery.h"

/**
 * @brief Load a single item from the cache.
 */
class GetItemQuery : public ItemsQuery
{
    Q_OBJECT
public:
    explicit GetItemQuery(QObject* parent = nullptr);

    QUuid uid() const;
    void setUid(const QUuid& uid);

signals:

    void itemLoaded(const QVariant& data, const QVariantList& parents, const QVariant& library);
    void itemNotFound();

public slots:

protected:
    // ItemsQuery interface
    void run() override;

private:
    QUuid m_uid;
};

#endif // DATASTORAGE_GETITEMQUERY_H_
