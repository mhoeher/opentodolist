#include "disposeaccount.h"

namespace OpenTodoList {
namespace DataBase {
namespace Queries {

DisposeAccount::DisposeAccount(Account *account) :
  DisposeObject<Account>( account )
{
  connect( this, &DisposeAccount::queryFinished,
           [this,account] {
    account->setDisposed( true );
    emit this->accountChanged( account->toVariant() );
  });
}

DisposeAccount::~DisposeAccount()
{

}

} // namespace Queries
} // namespace DataBase
} // namespace OpenTodoList

