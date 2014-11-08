/*
 *  OpenTodoList - A todo and task manager
 *  Copyright (C) 2013, 2014  Martin Höher <martin@rpdev.net>
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

#ifndef TODOLISTLIBRARY_H
#define TODOLISTLIBRARY_H

#include "todolist.h"
#include "pluginsloader.h"
#include "core/opentodolistinterfaces.h"
#include "database/database.h"

#include <QObject>
#include <QObjectList>
#include <QQmlListProperty>

namespace OpenTodoList {

namespace DataModel {

class TodoListLibrary : public QObject
{
    Q_OBJECT
    Q_PROPERTY( QString applicationVersion READ applicationVersion CONSTANT )
public:
    
    explicit TodoListLibrary(QObject *parent = 0);
    virtual ~TodoListLibrary();
    
    QString applicationVersion() const;

signals:
    
private:

};

} /* DataModel */

} /* OpenTodoList */

#endif // TODOLISTLIBRARY_H
