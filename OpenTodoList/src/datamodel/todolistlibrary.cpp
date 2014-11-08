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

#include "todolistlibrary.h"

#include "datamodel/todo.h"
#include "datamodel/todolist.h"

#include <QCoreApplication>
#include <QDebug>
#include <QSettings>

namespace OpenTodoList {

namespace DataModel {

TodoListLibrary::TodoListLibrary(QObject *parent) :
    QObject(parent)
{
    qDebug() << "Creating todo list library";

    // register basic types for thread communication
    qRegisterMetaType< ITodoList* >( "ITodoList*" );
    qRegisterMetaType< ITodo* >( "ITodo*" );
}

TodoListLibrary::~TodoListLibrary()
{
    qDebug() << "Deleting todo list library";
}

QString TodoListLibrary::applicationVersion() const
{
    return VERSION;
}

} /* DataModel */

} /* OpenTodoList */
