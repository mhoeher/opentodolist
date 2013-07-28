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


#ifndef OBJECTMODEL_H
#define OBJECTMODEL_H

#include "opentodolistcore_global.h"

#include <QObject>
#include <QAbstractListModel>
#include <QHash>

class OPENTODOLISTCORESHARED_EXPORT ObjectModelBase : public QAbstractListModel
{
    Q_OBJECT
public:

    typedef QList< QAbstractItemModel* > SubLists;

    explicit ObjectModelBase( QObject* parent = 0 );
    virtual ~ObjectModelBase();

    void appendList( QAbstractItemModel* subList );

    const SubLists& lists() const {
        return subLists;
    }

    Q_INVOKABLE int itemCount() const {
        return rowCount();
    }

    Q_INVOKABLE QVariantMap getItem( int index ) const;

signals:

protected:

    SubLists subLists;

    int subListEntryCount() const;

protected slots:

    void setThisItemCount( int thisItemCount );

private:

    int thisItemCount;

    void connectToSubModel( QAbstractItemModel* subList );

private slots:

    int listOffset(const QAbstractItemModel *subList ) const;
    QModelIndex fromSubList( const QModelIndex& index ) const;

    void onSubDataChanged(const QModelIndex & topLeft,
                          const QModelIndex & bottomRight );
    void onSubLayoutAboutToBeChanged();
    void onSubLayoutChanged();
    void onSubModelAboutToBeReset();
    void onSubModelReset();
    void onSubRowsAboutToBeInserted(const QModelIndex & parent, int start, int end);
    void onSubRowsAboutToBeMoved(const QModelIndex & sourceParent,
                                 int sourceStart, int sourceEnd,
                                 const QModelIndex & destinationParent,
                                 int destinationRow);
    void onSubRowsAboutToBeRemoved(const QModelIndex & parent,
                                   int start, int end);
    void onSubRowsInserted(const QModelIndex & parent, int start, int end);
    void onSubRowsMoved(const QModelIndex & sourceParent, int sourceStart,
                        int sourceEnd, const QModelIndex & destinationParent,
                        int destinationRow);
    void onSubRowsRemoved(const QModelIndex & parent, int start, int end);

};

template< typename ObjectType >
class OPENTODOLISTCORESHARED_EXPORT ObjectModel : public ObjectModelBase
{

public:
    
    typedef ObjectModel< ObjectType > ThisType;
    typedef QList< ObjectType* > List;
    
    enum {
        ObjectRole = Qt::UserRole + 1
    };
    
    ObjectModel( QObject* parent = 0 ) :
        ObjectModelBase( parent ),
        list( List() ),
        myRoleNames() {
        myRoleNames = QAbstractListModel::roleNames();
        myRoleNames.insert( ObjectRole, "object" );
    }

    virtual int rowCount(const QModelIndex& parent = QModelIndex()) const { 
        return parent.isValid() ? 0 : list.size() + this->subListEntryCount();
    }
        
    virtual QModelIndex index(int row, int column,
                              const QModelIndex& parent = QModelIndex()) const {
        return parent.isValid() ?
                    QModelIndex() :
                    this->createIndex( row, column, this->entryForRow( row ) );
    }
    
    virtual QVariant data(const QModelIndex& index,
                          int role = Qt::DisplayRole) const {
        switch ( role ) {
            case Qt::DisplayRole:
            case ObjectRole:
                return index.isValid() ?
                            QVariant::fromValue< QObject* >(
                                static_cast< QObject* >(
                                    index.internalPointer() ) ) :
                            QVariant();
            default:
                return QVariant();
        }
    }
    
    virtual QHash< int, QByteArray > roleNames() const {
        return myRoleNames;
    }
    
    void append( ObjectType* item ) {
        Q_ASSERT( item != 0 );
        beginInsertRows( QModelIndex(), rowCount(), rowCount() );
        list << item;
        endInsertRows();
        this->setThisItemCount( list.size() );
    }
    
    const List& data() const {
        return this->list;
    }
    
    void notifyObjectChanged( ObjectType* object ) {
        int idx = list.indexOf( object );
        if ( idx >= 0 ) {
            emit dataChanged( index( idx, 0 ), index( idx, 0 ) );
        }
    }
    
private:
    
    List list;
    QHash< int, QByteArray > myRoleNames;

    QObject* entryForRow( int row ) const {
        Q_ASSERT( row >= 0 );
        if ( row < this->list.size() ) {
            return (QObject*)( this->list.at( row ) );
        }
        row -= this->list.size();
        foreach ( QAbstractItemModel* subList, this->subLists ) {
            if ( row < subList->rowCount() ) {
                QModelIndex idx = subList->index( row, 0 );
                int role = ObjectRole;
                return idx.data( role ).value< QObject* >();
            }
            row -= subList->rowCount();
        }
        return 0;
    }

};

#endif // OBJECTMODEL_H
