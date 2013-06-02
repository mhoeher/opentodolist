/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) 2013  Martin Höher <martin@rpdev.net>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#ifndef OBJECTMODEL_H
#define OBJECTMODEL_H

#include "opentodolistcore_global.h"

#include <QAbstractListModel>
#include <QHash>

template< typename ObjectType >
class OPENTODOLISTCORESHARED_EXPORT ObjectModel : public QAbstractListModel
{
    
public:
    
    typedef QList< ObjectType* > List;
    
    enum {
        ObjectRole = Qt::UserRole + 1
    };
    
    ObjectModel( QObject* parent = 0 ) : QAbstractListModel( parent ), list( List() ), myRoleNames() {
        myRoleNames = QAbstractListModel::roleNames();
        myRoleNames.insert( ObjectRole, "object" );
    }

    virtual int rowCount(const QModelIndex& parent = QModelIndex()) const { 
        return parent.isValid() ? 0 : list.size(); 
    }
        
    virtual QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const {
        return parent.isValid() ? QModelIndex() : createIndex( row, column, list[row] );
    }
    
    virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const {
        switch ( role ) {
            case Qt::DisplayRole:
            case ObjectRole:
                return index.isValid() ? QVariant::fromValue< QObject* >( static_cast< QObject* >( index.internalPointer() ) ) : QVariant();
            default:
                return QVariant();
        }
    }
    
    virtual QHash< int, QByteArray > roleNames() const {
        return myRoleNames;
    }
    
    void append( ObjectType* item ) {
        beginInsertRows( QModelIndex(), rowCount(), rowCount() );
        list << item;
        endInsertRows();
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
    
};

#endif // OBJECTMODEL_H
