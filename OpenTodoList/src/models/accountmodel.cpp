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

#include "accountmodel.h"

#include "datamodel/objectinfo.h"
#include "database/databaseconnection.h"

namespace OpenTodoList {
namespace Models {

AccountModel::AccountModel(QObject *parent) :
    ObjectModel(ObjectInfo<Account>::classUuidProperty(), parent)
{
  connect( this, &AccountModel::objectAdded, [this] (QObject *object) {
    Account *account = dynamic_cast< Account* >( object );
    if ( account ) {
      this->connect( account, &Account::changed, [account,this] {
        if ( this->database() ) {
          DatabaseConnection conn;
          conn.setDatabase( this->database() );
          conn.insertAccount( account );
        }
      });
    }
  });
}

void AccountModel::connectToDatabase()
{
  connect( database(), &Database::accountChanged,
           this, &AccountModel::addAccount );
  connect( database(), &Database::accountDeleted,
           this, &AccountModel::removeAccount );
}

void AccountModel::disconnectFromDatabase()
{
  disconnect( database(), &Database::accountChanged,
              this, &AccountModel::addAccount );
  disconnect( database(), &Database::accountDeleted,
              this, &AccountModel::removeAccount );
}

StorageQuery *AccountModel::createQuery() const
{
  Queries::ReadAccount *query = new Queries::ReadAccount();
  connect( query, &Queries::ReadAccount::readAccount,
           this, &AccountModel::addAccount, Qt::QueuedConnection );
  return query;
}

void AccountModel::addAccount(const QVariant &account)
{
  this->addObject<Account>( account );
}

void AccountModel::removeAccount(const QVariant &account)
{
  this->removeObject<Account>(account);
}


} // namespace Models
} // namespace OpenTodoList
