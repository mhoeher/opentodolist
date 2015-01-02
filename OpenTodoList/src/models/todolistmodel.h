/*
 *  OpenTodoList - A todo and task manager
 *  Copyright (C) 2014 - 2015  Martin Höher <martin@rpdev.net>
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

#ifndef TODOLISTMODEL_H
#define TODOLISTMODEL_H

#include "models/private/objectmodel.h"

#include "core/opentodolistinterfaces.h"
#include "datamodel/todolist.h"

#include "database/database.h"

#include <QAbstractListModel>
#include <QPointer>
#include <QSet>
#include <QVariantMap>
#include <QVector>

namespace OpenTodoList {

namespace Models {

using namespace Models::Private;
using namespace DataModel;
using namespace DataBase;

class TodoListModel : public ObjectModel
{
    Q_OBJECT

public:

    explicit TodoListModel(QObject *parent = 0);
    virtual ~TodoListModel();

public slots:

    // ObjectModel interface
protected:
    void connectToDatabase();
    void disconnectFromDatabase();
    StorageQuery *createQuery() const;
    int compareObjects(QObject *left, QObject *right) const;

private:

    Database                       *m_database;
    QVector< DataModel::TodoList* > m_todoLists;

private slots:

    void addTodoList( const QVariant &todoList );
    void removeTodoList( const QVariant &todoList );
};

} /* Models */

} /* OpenTodoList */

#endif // TODOLISTMODEL_H
