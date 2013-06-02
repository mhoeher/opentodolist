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


#ifndef TODOSORTFILTERMODEL_H
#define TODOSORTFILTERMODEL_H

#include "objectmodel.h"

#include <QSortFilterProxyModel>

class AbstractTodo;

class TodoSortFilterModel : public QSortFilterProxyModel
{
    
    Q_OBJECT
    
public:
    
    typedef enum {
        TodoListEntries,
        SubTodos,
        NoFilter
    } FilterMode;
    
    typedef ObjectModel< AbstractTodo > TodoModel;
    
    explicit TodoSortFilterModel( QObject* parent = 0 );
    
    void setSourceModel(TodoModel* sourceModel);
    
    FilterMode filterMode() const {
        return m_filterMode;
    }
    
    void setFilterMode( FilterMode mode ) {
        emit beginResetModel();
        m_filterMode = mode;
        emit endResetModel();
    }
    
    AbstractTodo* parentTodo() const {
        return m_parentTodo;
    }
    
    void setParentTodo( AbstractTodo* todo );
    
    virtual QHash< int, QByteArray > roleNames() const {
        return m_model ? m_model->roleNames() : QAbstractProxyModel::roleNames();
    }

protected:
    
    virtual bool filterAcceptsRow(int source_row, 
                                  const QModelIndex& source_parent) const;
    
private:
    
    FilterMode m_filterMode;
    AbstractTodo *m_parentTodo;
    TodoModel* m_model;
    
    virtual void setSourceModel(QAbstractItemModel*) {}
    
};

#endif // TODOSORTFILTERMODEL_H
