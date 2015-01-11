/*
 *  OpenTodoList - A todo and task manager
 *  Copyright (C) 2014 - 2015  Martin Höher <martin@rpdev.net>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef OPENTODOLIST_DATABASE_QUERIES_INSERTACCOUNT_H
#define OPENTODOLIST_DATABASE_QUERIES_INSERTACCOUNT_H

#include "datamodel/account.h"

#include "database/storagequery.h"
#include "database/queries/private/insertobject.h"

#include <QList>

namespace OpenTodoList {
namespace DataBase {
namespace Queries {

using namespace Private;
using namespace DataModel;

/**
   @brief Inserts a single account into the database

   This query can be used to insert a single DataModel::Account object into the Database.
 */
class InsertAccount : public InsertObject<DataModel::Account>
{
    Q_OBJECT
public:

    explicit InsertAccount( Account *account, bool update );
    virtual ~InsertAccount();

signals:

public slots:

private:

};

} // namespace Queries
} // namespace DataBase
} // namespace OpenTodoList

#endif // OPENTODOLIST_DATABASE_QUERIES_INSERTACCOUNT_H
