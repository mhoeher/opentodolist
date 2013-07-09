/*
 *  OpenTodoListCore - Core functionality for the OpenTodoList application
 *  Copyright (C) 2013  Martin Höher <martin@rpdev.net>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "todosortfiltermodel.h"

#include "abstracttodo.h"

TodoSortFilterModel::TodoSortFilterModel(QObject* parent): 
    QSortFilterProxyModel(parent),
    m_filterMode( NoFilter ),
    m_sortMode( NoSort ),
    m_parentTodo( 0 ),
    m_searchString( QString() ),
    m_maxDueDate( QDateTime() )
{
}

void TodoSortFilterModel::setSourceModel(TodoSortFilterModel::TodoModel* sourceModel)
{
    QSortFilterProxyModel::setSourceModel( sourceModel );
    emit sourceModelChanged();
}

void TodoSortFilterModel::setSourceModel(TodoSortFilterModel* sourceModel)
{
    QSortFilterProxyModel::setSourceModel( sourceModel );
    emit sourceModelChanged();
}

void TodoSortFilterModel::setSourceModel(QObject *sourceModel)
{
    QSortFilterProxyModel::setSourceModel(
                qobject_cast< QAbstractItemModel* >( sourceModel ) );
    emit sourceModelChanged();
}

void TodoSortFilterModel::setParentTodo(AbstractTodo* todo)
{
    if ( m_filterMode == SubTodos ) {
        emit beginResetModel();
        m_parentTodo = todo;
        emit endResetModel();
    } else {
        m_parentTodo = todo;
    }
}

bool TodoSortFilterModel::filterAcceptsRow(int source_row, const QModelIndex& source_parent) const
{
    bool result = QSortFilterProxyModel::filterAcceptsRow(source_row, source_parent);
    AbstractTodo* todo = qobject_cast< AbstractTodo* >(
                sourceModel()->index( source_row, 0 ).data(
                    TodoModel::ObjectRole ).value< QObject* >() );
    if ( todo ) {
        if ( !m_searchString.isEmpty() ) {
            if ( !(todo->title().contains( m_searchString, Qt::CaseInsensitive ) ||
                 todo->description().contains( m_searchString, Qt::CaseInsensitive )) ) {
                result = false;
            }
        }
        if ( m_maxDueDate.isValid() ) {
            result = !todo->isCompleted() && todo->dueDate().isValid() &&
                    todo->dueDate().daysTo( m_maxDueDate ) >= 0;
        }
        if ( m_filterMode & TodoListEntries ) {
            result = !todo->parentTodo() ? result : false;
        }
        if ( m_filterMode & SubTodos ) {
            result = todo->parentTodo() == m_parentTodo ? result : false;
        }
        if ( m_filterMode & HideDeleted ) {
            result = todo->isDeleted() ? false : result;
        }
        if ( m_filterMode & HideNonDeleted ) {
            result = todo->isDeleted() ? result : false;
        }
    }
    return result;
}

bool TodoSortFilterModel::lessThan(const QModelIndex& left, const QModelIndex& right) const
{
    if ( m_sortMode != NoSort ) {
        AbstractTodo* leftTodo = qobject_cast< AbstractTodo* >(
                    left.data( TodoModel::ObjectRole ).value<QObject*>());
        AbstractTodo* rightTodo = qobject_cast< AbstractTodo* >(
                    right.data( TodoModel::ObjectRole ).value< QObject* >());
        if ( leftTodo->isCompleted() && !rightTodo->isCompleted() ) {
            return false;
        }
        if ( !leftTodo->isCompleted() && rightTodo->isCompleted() ) {
            return true;
        }
        if ( leftTodo && rightTodo ) {
            switch ( m_sortMode ) {
            case PrioritySort:
                return leftTodo->priority() > rightTodo->priority();
            case ProgressSort:
                return leftTodo->progress() < rightTodo->progress();
            default:
                return QSortFilterProxyModel::lessThan( left, right );
            }
        }
    }
    return QSortFilterProxyModel::lessThan(left, right);
}
