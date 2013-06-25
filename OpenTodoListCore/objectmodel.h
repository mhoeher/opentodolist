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

template< typename ObjectType >
class OPENTODOLISTCORESHARED_EXPORT ObjectModel : public QAbstractListModel
{

public:
    
    typedef ObjectModel< ObjectType > ThisType;
    typedef QList< ObjectType* > List;
    typedef QList< QAbstractListModel* > SubLists;
    
    enum {
        ObjectRole = Qt::UserRole + 1
    };
    
    ObjectModel( QObject* parent = 0 ) :
        QAbstractListModel( parent ),
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
                    createIndex( row, column, this->entryForRow( row ) );
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
    }

    void append( QAbstractListModel* subList ) {
        Q_ASSERT( subList != 0 );
        beginInsertRows( QModelIndex(), rowCount(),
                         rowCount() + subList->rowCount() );
        subLists << subList;
        connect( subList, SIGNAL(dataChanged(QModelIndex,QModelIndex)),
                 this, SLOT(onChildDataChanged(QModelIndex,QModelIndex)) );
        endInsertRows();
    }
    
    const List& data() const {
        return this->list;
    }

    const SubLists& lists() const {
        return this->subLists();
    }
    
    void notifyObjectChanged( ObjectType* object ) {
        int idx = list.indexOf( object );
        if ( idx >= 0 ) {
            emit dataChanged( index( idx, 0 ), index( idx, 0 ) );
        }
    }
    
private:
    
    List list;
    SubLists subLists;
    QHash< int, QByteArray > myRoleNames;

    int subListEntryCount() const {
        int result = 0;
        foreach ( QAbstractListModel* subList, this->subLists ) {
            result += subList->rowCount();
        }
        return result;
    }

    QObject* entryForRow( int row ) const {
        Q_ASSERT( row >= 0 );
        if ( row < this->list.size() ) {
            return qobject_cast< QObject* >( this->list.at( row ) );
        }
        row -= this->list.size();
        foreach ( QAbstractListModel* subList, this->subLists ) {
            if ( row <= subList->rowCount() ) {
                QModelIndex idx = subList->index( row );
                int role = ObjectRole;
                return idx.data( role ).value< QObject* >();
            }
            row -= subList->rowCount();
        }
        return 0;
    }
    
private slots:

    void onChildDataChanged( const QModelIndex& topLeft,
                             const QModelIndex& bottomRight ) {

    }
};

#endif // OBJECTMODEL_H
