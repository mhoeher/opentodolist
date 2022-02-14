/*
 * Copyright 2020 Martin Hoeher <martin@rpdev.net>
 +
 * This file is part of OpenTodoList.
 *
 * OpenTodoList is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * OpenTodoList is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with OpenTodoList.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "models/itemssortfiltermodel.h"

#include <QUuid>

#include "itemsmodel.h"

#include "datamodel/item.h"
#include "datamodel/library.h"
#include "datamodel/task.h"
#include "datamodel/todo.h"
#include "datamodel/todolist.h"

ItemsSortFilterModel::ItemsSortFilterModel(QObject* parent)
    : QSortFilterProxyModel(parent), m_groupDone(false)
{
    setSortRole(ItemsModel::WeightRole);
    sort(0); // NOLINT

    connect(this, &ItemsSortFilterModel::rowsInserted, this, &ItemsSortFilterModel::countChanged);
    connect(this, &ItemsSortFilterModel::rowsRemoved, this, &ItemsSortFilterModel::countChanged);
    connect(this, &ItemsSortFilterModel::modelReset, this, &ItemsSortFilterModel::countChanged);
}

/**
 * @brief The number of items in the model.
 */
int ItemsSortFilterModel::count() const
{
    return rowCount();
}

/**
 * @brief Get the role from the role name.
 */
int ItemsSortFilterModel::roleFromName(const QString& roleName) const
{
    return roleNames().key(roleName.toUtf8(), -1);
}

bool ItemsSortFilterModel::lessThan(const QModelIndex& source_left,
                                    const QModelIndex& source_right) const
{
    if (m_groupDone) {
        auto leftDone = source_left.data(ItemsModel::DoneRole).toBool();
        auto rightDone = source_right.data(ItemsModel::DoneRole).toBool();
        if (!leftDone && rightDone) {
            // Left is undone, right is done - it is clearly "less than".
            return true;
        }
        if (leftDone && !rightDone) {
            // Left is done, right undone - it is clearly "not less than".
            return false;
        }
        // In any other case, we check out the next sort criteria.
    }

    switch (sortRole()) {

    // For the following roles, sort in reverse order (usually, to get a
    // "most-recent on top" ordering):
    case ItemsModel::CreatedAtRole:
    case ItemsModel::UpdatedAtRole:
    case ItemsModel::EffectiveUpdatedAtRole:
        return QSortFilterProxyModel::lessThan(source_right, source_left);

    // For the DueTo role, apply a little trick: Sort by the due to role
    // data (converted to a string), but append an 'X'. This causes
    // Any items with a valid due date to appear first in listings.
    case ItemsModel::DueToRole:
    case ItemsModel::EffectiveDueToRole: {
        auto left_dt = source_left.data(sortRole()).toString();
        auto right_dt = source_right.data(sortRole()).toString();
        return left_dt + "x" < right_dt + "x";
    }

    // For everything else, use the default sorting:
    default:
        return QSortFilterProxyModel::lessThan(source_left, source_right);
    }
}

/**
 * @brief Group items that are done together.
 */
bool ItemsSortFilterModel::groupDone() const
{
    return m_groupDone;
}

void ItemsSortFilterModel::setGroupDone(bool newGroupDone)
{
    if (m_groupDone == newGroupDone)
        return;
    m_groupDone = newGroupDone;
    emit groupDoneChanged();
    invalidate();
}
