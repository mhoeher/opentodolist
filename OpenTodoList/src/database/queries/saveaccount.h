#ifndef OPENTODOLIST_DATABASE_QUERIES_SAVEACCOUNT_H
#define OPENTODOLIST_DATABASE_QUERIES_SAVEACCOUNT_H

#include "datamodel/account.h"
#include "database/queries/private/saveobject.h"

namespace OpenTodoList {
namespace DataBase {
namespace Queries {

using namespace Private;
using namespace DataModel;

class SaveAccount : public SaveObject<Account>
{
public:
  SaveAccount( Account *account );
  ~SaveAccount();
};

} // namespace Queries
} // namespace DataBase
} // namespace OpenTodoList

#endif // OPENTODOLIST_DATABASE_QUERIES_SAVEACCOUNT_H
