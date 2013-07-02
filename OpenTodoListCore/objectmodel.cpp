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

#include "objectmodel.h"

ObjectModelBase::ObjectModelBase(QObject *parent) :
    QAbstractListModel( parent ),
    subLists( SubLists() ),
    thisItemCount( 0 )
{
}

ObjectModelBase::~ObjectModelBase()
{
}

void ObjectModelBase::appendList(QAbstractItemModel *subList)
{
    Q_ASSERT( subList != 0 );
    bool notifyInsert = subList->rowCount() > 0;
    if ( notifyInsert ) {
        beginInsertRows( QModelIndex(), rowCount(),
                         rowCount() + subList->rowCount() );
    }
    subLists << subList;
    if ( notifyInsert ) {
        endInsertRows();
    }
    connectToSubModel( subList );
}

void ObjectModelBase::setThisItemCount(int thisItemCount)
{
    this->thisItemCount = thisItemCount;
}

void ObjectModelBase::connectToSubModel( QAbstractItemModel* subList )
{
    connect( subList, SIGNAL(dataChanged(QModelIndex,QModelIndex)),
             this, SLOT(onSubDataChanged(QModelIndex,QModelIndex)) );
    connect( subList, SIGNAL(layoutAboutToBeChanged()),
             this, SLOT(onSubLayoutAboutToBeChanged()) );
    connect( subList, SIGNAL(layoutChanged()),
             this, SLOT(onSubLayoutChanged()) );
    connect( subList, SIGNAL(modelAboutToBeReset()),
             this, SLOT(onSubModelAboutToBeReset()) );
    connect( subList, SIGNAL(modelReset()),
             this, SLOT(onSubModelReset()) );
    connect( subList, SIGNAL(rowsAboutToBeInserted(QModelIndex,int,int)),
            this, SLOT(onSubRowsAboutToBeInserted(QModelIndex,int,int)) );
    connect( subList, SIGNAL(rowsInserted(QModelIndex,int,int)),
             this, SLOT(onSubRowsInserted(QModelIndex,int,int)) );
    connect( subList, SIGNAL(rowsAboutToBeMoved(QModelIndex,int,int,QModelIndex,int)),
             this, SLOT(onSubRowsAboutToBeMoved(QModelIndex,int,int,QModelIndex,int)) );
    connect( subList, SIGNAL(rowsMoved(QModelIndex,int,int,QModelIndex,int)),
             this, SLOT(onSubRowsMoved(QModelIndex,int,int,QModelIndex,int)) );
    connect( subList, SIGNAL(rowsAboutToBeRemoved(QModelIndex,int,int)),
             this, SLOT(onSubRowsAboutToBeRemoved(QModelIndex,int,int)) );
    connect( subList, SIGNAL(rowsRemoved(QModelIndex,int,int)),
             this, SLOT(onSubRowsRemoved(QModelIndex,int,int)) );
}

int ObjectModelBase::listOffset(const QAbstractItemModel *subList) const
{
    int result = thisItemCount;
    foreach ( QAbstractItemModel* list, subLists ) {
        if ( subList == list ) {
            return result;
        } else {
            result += list->rowCount();
        }
    }
    return 0;
}

QModelIndex ObjectModelBase::fromSubList(const QModelIndex &index) const
{
    if ( index.isValid() && index.model() ) {
        return this->index( listOffset( index.model() ) + index.row(), index.column() );
    }
    return QModelIndex();
}

void ObjectModelBase::onSubDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight)
{
    emit dataChanged( fromSubList( topLeft ), fromSubList( bottomRight ) );
}

void ObjectModelBase::onSubLayoutAboutToBeChanged()
{
    emit beginResetModel();
}

void ObjectModelBase::onSubLayoutChanged()
{
    emit endResetModel();
}

void ObjectModelBase::onSubModelAboutToBeReset()
{
    emit beginResetModel();
}

void ObjectModelBase::onSubModelReset()
{
    emit endResetModel();
}

void ObjectModelBase::onSubRowsAboutToBeInserted(const QModelIndex &parent,
                                                 int start, int end)
{
    QAbstractItemModel* subModel = qobject_cast< QAbstractItemModel* >( sender() );
    if ( subModel ) {
        int offset = listOffset( subModel );
        emit beginInsertRows( fromSubList( parent ), offset + start, offset + end );
    }
}

void ObjectModelBase::onSubRowsAboutToBeMoved(const QModelIndex &sourceParent,
                                              int sourceStart, int sourceEnd,
                                              const QModelIndex &destinationParent,
                                              int destinationRow)
{
    QAbstractItemModel* subModel = qobject_cast< QAbstractItemModel* >( sender() );
    if ( subModel ) {
        int offset = listOffset( subModel );
        emit beginMoveRows( fromSubList( sourceParent ), offset + sourceStart,
                            offset + sourceEnd,
                            fromSubList( destinationParent ),
                            offset + destinationRow );
    }
}

void ObjectModelBase::onSubRowsAboutToBeRemoved(const QModelIndex &parent,
                                                int start, int end)
{
    QAbstractItemModel* subModel = qobject_cast< QAbstractItemModel* >( sender() );
    if ( subModel ) {
        int offset = listOffset( subModel );
        emit beginRemoveRows( fromSubList( parent ),
                              offset + start, offset + end );
    }
}

void ObjectModelBase::onSubRowsInserted(const QModelIndex &parent, int start, int end)
{
    Q_UNUSED( parent );
    Q_UNUSED( start );
    Q_UNUSED( end );
    emit endInsertRows();
}

void ObjectModelBase::onSubRowsMoved(const QModelIndex &sourceParent,
                                     int sourceStart, int sourceEnd,
                                     const QModelIndex &destinationParent,
                                     int destinationRow)
{
    Q_UNUSED( sourceParent );
    Q_UNUSED( sourceStart );
    Q_UNUSED( sourceEnd );
    Q_UNUSED( destinationParent );
    Q_UNUSED( destinationRow );
    emit endMoveRows();
}

void ObjectModelBase::onSubRowsRemoved(const QModelIndex &parent, int start, int end)
{
    Q_UNUSED( parent );
    Q_UNUSED( start );
    Q_UNUSED( end );
    emit endRemoveRows();
}

int ObjectModelBase::subListEntryCount() const
{
    int result = 0;
    foreach ( QAbstractItemModel* subList, this->subLists ) {
        result += subList->rowCount();
    }
    return result;
}

#include "objectmodel.h"

