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
