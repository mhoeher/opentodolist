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
