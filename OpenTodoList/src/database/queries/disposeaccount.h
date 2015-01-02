#ifndef OPENTODOLIST_DATABASE_QUERIES_DISPOSEACCOUNT_H
#define OPENTODOLIST_DATABASE_QUERIES_DISPOSEACCOUNT_H

#include "datamodel/account.h"
#include "database/queries/private/disposeobject.h"

namespace OpenTodoList {
namespace DataBase {
namespace Queries {

using namespace Private;
using namespace DataModel;

class DisposeAccount : public DisposeObject<Account>
{
public:
  DisposeAccount( Account *account );
  ~DisposeAccount();
};

} // namespace Queries
} // namespace DataBase
} // namespace OpenTodoList

#endif // OPENTODOLIST_DATABASE_QUERIES_DISPOSEACCOUNT_H
