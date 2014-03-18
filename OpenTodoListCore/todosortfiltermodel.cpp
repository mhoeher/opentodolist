/*
 *  OpenTodoList - A todo and task manager
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

#include "todo.h"

TodoSortFilterModel::TodoSortFilterModel(QObject* parent): 
    QSortFilterProxyModel(parent),
    m_filterMode( NoFilter ),
    m_sortMode( NoSort ),
    m_parentTodo( 0 ),
    m_searchString( QString() ),
    m_maxDueDate( QDateTime() ),
    m_minDueDate( QDateTime() )
{
    setDynamicSortFilter( true );
    sort( 0, Qt::AscendingOrder );
    connect( this, SIGNAL(rowsInserted(QModelIndex,int,int)),
             this, SIGNAL(itemCountChanged()) );
    connect( this, SIGNAL(rowsRemoved(QModelIndex,int,int)),
             this, SIGNAL(itemCountChanged()) );
    connect( this, SIGNAL(modelReset()),
             this, SIGNAL(itemCountChanged()) );
    connect( this, SIGNAL(layoutChanged()),
             this, SIGNAL(itemCountChanged()) );
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

void TodoSortFilterModel::setParentTodo(Todo* todo)
{
    if ( m_filterMode == SubTodos ) {
        emit beginResetModel();
        m_parentTodo = todo;
        emit endResetModel();
    } else {
        m_parentTodo = todo;
    }
}

QVariantMap TodoSortFilterModel::getItem(int index) const
{
    QVariantMap result;
    const QHash<int, QByteArray> &roles = this->roleNames();
    foreach ( int key, roles.keys() ) {
        result.insert( roles.value( key ), this->index( index, 0 ).data( key ) );
    }
    return result;
}

bool TodoSortFilterModel::filterAcceptsRow(int source_row, const QModelIndex& source_parent) const
{
    bool result = QSortFilterProxyModel::filterAcceptsRow(source_row, source_parent);
    Todo* todo = qobject_cast< Todo* >(
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
        if ( m_minDueDate.isValid() ) {
            result = !todo->isCompleted() && todo->dueDate().isValid() &&
                    todo->dueDate().daysTo( m_minDueDate ) <= 0;
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
    Todo* leftTodo = qobject_cast< Todo* >(
                left.data( TodoModel::ObjectRole ).value<QObject*>());
    Todo* rightTodo = qobject_cast< Todo* >(
                right.data( TodoModel::ObjectRole ).value< QObject* >());
    if ( m_sortMode != NoSort ) {
        if ( leftTodo && rightTodo ) {
            if ( leftTodo->isCompleted() && !rightTodo->isCompleted() ) {
                return false;
            }
            if ( !leftTodo->isCompleted() && rightTodo->isCompleted() ) {
                return true;
            }
            switch ( m_sortMode ) {
            case PrioritySort:
                if ( leftTodo->priority() == rightTodo->priority() ) {
                    return leftTodo->title() < rightTodo->title();
                } else {
                    return leftTodo->priority() > rightTodo->priority();
                }
            case ProgressSort:
                if ( leftTodo->progress() == rightTodo->progress() ) {
                    return leftTodo->title() == rightTodo->title();
                } else {
                    return leftTodo->progress() < rightTodo->progress();
                }
            default:
                return leftTodo->title() < rightTodo->title();
            }
        }
    }
    return leftTodo && rightTodo ? leftTodo->title() < rightTodo->title() : true;
}

