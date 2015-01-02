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

#include "insertaccount.h"

namespace OpenTodoList {
namespace DataBase {
namespace Queries {

/**
   @brief Constructor

   Creates a new InsertAccount object, which will upon run insert or update the
   @p account in the Database. The insertion is influenced by the @p flags. See
   documentation for InsertFlag.

   @note By default, the query takes over ownership of the account. Pass
         StorageQuery::CallerIsOwner as @p ownership to keep ownership untouched.
 */
InsertAccount::InsertAccount(Account *account, bool update) :
    OpenTodoList::DataBase::Queries::Private::InsertObject<Account>(
      account,
      { "uuid", "name" },
      update )
{
  connect( this, &InsertAccount::queryFinished,
           [this,account] { emit this->accountChanged( account->toVariant() ); } );
}

/**
   @brief Destructor
 */
InsertAccount::~InsertAccount()
{
}

} // namespace Queries
} // namespace DataBase
} // namespace OpenTodoList
