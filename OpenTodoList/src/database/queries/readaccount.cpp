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

#include "readaccount.h"

namespace OpenTodoList {
namespace DataBase {
namespace Queries {

/**
   @brief Constructor

   Creates a new query. If @p parentizeAccounts is true, the query takes ownership
   of the created account objects. Otherwise, either use accounts() or connect to readAccount()
   to collect the created accounts.
 */
ReadAccount::ReadAccount() :
    ReadObject<Account>( { "uuid", "name" } )
{
  connect( this, &ReadAccount::queryFinished,
           [this] {
    for ( Account* account : this->objects() ) {
      emit this->readAccount( account->toVariant() );
    }
  } );
}

/**
   @brief The list of accounts read from the database
 */
QList<DataModel::Account *> ReadAccount::accounts() const
{
    return objects();
}

} // namespace Queries
} // namespace DataBase
} // namespace OpenTodoList
