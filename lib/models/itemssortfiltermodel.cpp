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
    sort(0);

    connect(this, &ItemsSortFilterModel::rowsInserted,
            this, &ItemsSortFilterModel::countChanged);
    connect(this, &ItemsSortFilterModel::rowsRemoved,
            this, &ItemsSortFilterModel::countChanged);
    connect(this, &ItemsSortFilterModel::modelReset,
            this, &ItemsSortFilterModel::countChanged);
}


/**
 * @brief The number of items in the model.
 */
int ItemsSortFilterModel::count() const
{
    return rowCount();
}

bool ItemsSortFilterModel::lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const
{
    switch (sortRole()) {

    // For the following roles, sort in reverse order (usually, to get a
    // "most-recent on top" ordering):
    case ItemsModel::CreatedAtRole:
    case ItemsModel::UpdatedAtRole:
        return QSortFilterProxyModel::lessThan(source_right, source_left);

    // For the DueTo role, apply a little trick: Sort by the due to role
    // data (converted to a string), but append an 'X'. This causes
    // Any items with a valid due date to appear first in listings.
    case ItemsModel::DueToRole:
    {
        auto left_dt = source_left.data(ItemsModel::DueToRole).toString();
        auto right_dt = source_right.data(ItemsModel::DueToRole).toString();
        return left_dt + "x" < right_dt + "x";
    }

    // For everything else, use the default sorting:
    default:
        return QSortFilterProxyModel::lessThan(source_left, source_right);
    }
}
