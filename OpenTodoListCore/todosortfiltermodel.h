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
#include "opentodolistcore_global.h"

#include <QDateTime>
#include <QSortFilterProxyModel>

class AbstractTodo;

class OPENTODOLISTCORESHARED_EXPORT TodoSortFilterModel : public QSortFilterProxyModel
{
    
    Q_OBJECT
    Q_ENUMS( FilterMode )
    Q_ENUMS( SortMode )
    Q_PROPERTY( QObject* sourceModel READ sourceModel WRITE setSourceModel NOTIFY sourceModelChanged )
    Q_PROPERTY( int filterMode READ filterMode WRITE setFilterMode NOTIFY filterModeChanged )
    Q_PROPERTY( SortMode sortMode READ sortMode WRITE setSortMode NOTIFY sortModeChanged )
    Q_PROPERTY( QString searchString READ searchString WRITE setSearchString NOTIFY searchStringChanged )
    Q_PROPERTY( QDateTime maxDueDate READ maxDueDate WRITE setMaxDueDate NOTIFY maxDueDateChanged )
    Q_PROPERTY( QDateTime minDueDate READ minDueDate WRITE setMinDueDate NOTIFY minDueDateChanged )
    
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
    void setSourceModel( QObject* sourceModel );
    
    int filterMode() const {
        return m_filterMode;
    }
    
    void setFilterMode( int mode ) {
        m_filterMode = mode;
        invalidateFilter();
        emit filterModeChanged();
    }
    
    SortMode sortMode() const {
        return m_sortMode;
    }
    
    void setSortMode( SortMode sortMode ) {
        emit beginResetModel();
        m_sortMode = sortMode;
        emit endResetModel();
        emit sortModeChanged();
    }
    
    AbstractTodo* parentTodo() const {
        return m_parentTodo;
    }

    void setSearchString( const QString& searchString ) {
        m_searchString = searchString;
        invalidateFilter();
        emit searchStringChanged();
    }

    QString searchString() const {
        return m_searchString;
    }

    QDateTime maxDueDate() const {
        return m_maxDueDate;
    }

    void setMaxDueDate( const QDateTime& maxDueDate ) {
        m_maxDueDate = maxDueDate;
        invalidateFilter();
        emit maxDueDateChanged();
    }

    QDateTime minDueDate() const {
        return m_minDueDate;
    }

    void setMinDueDate( const QDateTime& minDueDate ) {
        m_minDueDate = minDueDate;
        invalidateFilter();
        emit minDueDateChanged();
    }
    
    void setParentTodo( AbstractTodo* todo );
    
    virtual QHash< int, QByteArray > roleNames() const {
        return sourceModel() ? sourceModel()->roleNames() : QAbstractProxyModel::roleNames();
    }

signals:

    void sourceModelChanged();
    void sortModeChanged();
    void filterModeChanged();
    void searchStringChanged();
    void maxDueDateChanged();
    void minDueDateChanged();

protected:
    
    virtual bool filterAcceptsRow(int source_row, 
                                  const QModelIndex& source_parent) const;
    virtual bool lessThan(const QModelIndex& left, const QModelIndex& right) const;
    
private:
    
    int m_filterMode;
    SortMode m_sortMode;
    AbstractTodo *m_parentTodo;
    QString m_searchString;
    QDateTime m_maxDueDate;
    QDateTime m_minDueDate;

    virtual void setSourceModel(QAbstractItemModel*) {}
    
};

#endif // TODOSORTFILTERMODEL_H
