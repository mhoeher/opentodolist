#include "deleteaccount.h"

namespace OpenTodoList {
namespace DataBase {
namespace Queries {

DeleteAccount::DeleteAccount(Account *account) :
  DeleteObject<Account>( account )
{
  connect( this, &DeleteAccount::queryFinished,
           [this,account] {
    emit this->accountDeleted( account->toVariant() );
  });
}

DeleteAccount::~DeleteAccount()
{
}

} // namespace Queries
} // namespace DataBase
} // namespace OpenTodoList

