#ifndef OPENTODOLIST_DATABASE_QUERIES_DELETEACCOUNT_H
#define OPENTODOLIST_DATABASE_QUERIES_DELETEACCOUNT_H

#include "datamodel/account.h"
#include "database/queries/private/deleteobject.h"

namespace OpenTodoList {
namespace DataBase {
namespace Queries {

using namespace Private;
using namespace DataModel;

class DeleteAccount : public DeleteObject<Account>
{
public:
  DeleteAccount( Account *account );
  ~DeleteAccount();
};

} // namespace Queries
} // namespace DataBase
} // namespace OpenTodoList

#endif // OPENTODOLIST_DATABASE_QUERIES_DELETEACCOUNT_H
