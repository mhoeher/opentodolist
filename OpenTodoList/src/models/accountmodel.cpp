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
