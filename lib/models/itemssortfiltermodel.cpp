#include "itemssortfiltermodel.h"

#include <QUuid>

#include "itemsmodel.h"

#include "datamodel/item.h"
#include "datamodel/library.h"
#include "datamodel/task.h"
#include "datamodel/todo.h"
#include "datamodel/todolist.h"

ItemsSortFilterModel::ItemsSortFilterModel(QObject *parent) :
    QSortFilterProxyModel(parent)
{
    setSortRole(ItemsModel::WeightRole);
}


/**
 * @brief The number of items in the model.
 */
int ItemsSortFilterModel::count() const
{
    return rowCount();
}


void ItemsSortFilterModel::setSourceModel(QAbstractItemModel *sourceModel)
{

}
