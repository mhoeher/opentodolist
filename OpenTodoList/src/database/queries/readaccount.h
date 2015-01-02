/*
 *  OpenTodoList - A todo and task manager
 *  Copyright (C) 2014 - 2015 Martin Höher <martin@rpdev.net>
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

#ifndef OPENTODOLIST_DATABASE_QUERIES_READACCOUNT_H
#define OPENTODOLIST_DATABASE_QUERIES_READACCOUNT_H

#include "datamodel/account.h"

#include "database/storagequery.h"
#include "database/queries/private/readobject.h"

#include <QList>

namespace OpenTodoList {
namespace DataBase {
namespace Queries {

using namespace Private;
using namespace DataModel;

class ReadAccount : public ReadObject<Account>
{
    Q_OBJECT
public:
    explicit ReadAccount();

    QList< DataModel::Account* > accounts() const;

signals:

    /**
       @brief An account has been read from the database

       This signal is emitted for every @p account read from the database. Use
       Account::fromVariant() to convert the variant to an Account object.
     */
    void readAccount( const QVariant &account );

public slots:

private:

};

} // namespace Queries
} // namespace DataBase
} // namespace OpenTodoList

#endif // OPENTODOLIST_DATABASE_QUERIES_READACCOUNT_H
