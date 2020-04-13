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

#ifndef DATASTORAGE_GETLIBRARYQUERY_H_
#define DATASTORAGE_GETLIBRARYQUERY_H_

#include <QObject>
#include <QUuid>

#include "datastorage/itemsquery.h"

class GetLibraryQuery : public ItemsQuery
{
    Q_OBJECT
public:
    explicit GetLibraryQuery(QObject *parent = nullptr);

    QUuid uid() const;
    void setUid(const QUuid &uid);

signals:

    void libraryLoaded(const QVariant &data);

public slots:

protected:
    // ItemsQuery interface
    void run() override;

private:
    QUuid m_uid;
};

#endif // DATASTORAGE_GETLIBRARYQUERY_H_
