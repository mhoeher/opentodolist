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


#ifndef TODOSORTFILTERMODEL_H
#define TODOSORTFILTERMODEL_H

#include "objectmodel.h"

#include <QSortFilterProxyModel>

class AbstractTodo;

class TodoSortFilterModel : public QSortFilterProxyModel
{
    
    Q_OBJECT
    Q_ENUMS( SortMode )
    
public:
    
    typedef enum {
        TodoListEntries     = 0x0001,
        SubTodos            = 0x0002,
        HideDeleted         = 0x0004,
        HideNonDeleted      = 0x0008,
        NoFilter            = 0
    } FilterMode;

    typedef enum {
        NoSort,
        PrioritySort,
        ProgressSort
    } SortMode;
    
    typedef ObjectModel< AbstractTodo > TodoModel;
    
    explicit TodoSortFilterModel( QObject* parent = 0 );
    
    void setSourceModel(TodoModel* sourceModel);
    void setSourceModel( TodoSortFilterModel* sourceModel );
    
    int filterMode() const {
        return m_filterMode;
    }
    
    void setFilterMode( int mode ) {
        emit beginResetModel();
        m_filterMode = mode;
        emit endResetModel();
    }
    
    SortMode sortMode() const {
        return m_sortMode;
    }
    
    void setSortMode( SortMode sortMode ) {
        emit beginResetModel();
        m_sortMode = sortMode;
        emit endResetModel();
    }
    
    AbstractTodo* parentTodo() const {
        return m_parentTodo;
    }
    
    void setParentTodo( AbstractTodo* todo );
    
    virtual QHash< int, QByteArray > roleNames() const {
        return sourceModel() ? sourceModel()->roleNames() : QAbstractProxyModel::roleNames();
    }

protected:
    
    virtual bool filterAcceptsRow(int source_row, 
                                  const QModelIndex& source_parent) const;
    virtual bool lessThan(const QModelIndex& left, const QModelIndex& right) const;
    
private:
    
    int m_filterMode;
    SortMode m_sortMode;
    AbstractTodo *m_parentTodo;
    
    virtual void setSourceModel(QAbstractItemModel*) {}
    
};

#endif // TODOSORTFILTERMODEL_H
