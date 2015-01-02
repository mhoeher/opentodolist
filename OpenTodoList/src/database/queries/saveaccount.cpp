#include "saveaccount.h"

namespace OpenTodoList {
namespace DataBase {
namespace Queries {

SaveAccount::SaveAccount(Account *account) :
  SaveObject<Account>( account )
{
}

SaveAccount::~SaveAccount()
{

}

} // namespace Queries
} // namespace DataBase
} // namespace OpenTodoList

