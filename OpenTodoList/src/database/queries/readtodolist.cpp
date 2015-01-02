/*
 *  OpenTodoList - A todo and task manager
 *  Copyright (C) 2014 - 2015 Martin Höher <martin@rpdev.net>
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

#include "readtodolist.h"

namespace OpenTodoList {
namespace DataBase {
namespace Queries {

ReadTodoList::ReadTodoList() :
    ReadObject<TodoList>( { "uuid", "name" } )
{
  connect( this, &ReadTodoList::queryFinished,
           [this] {
    for ( TodoList *todoList : this->objects() ) {
      emit this->readTodoList( todoList->toVariant() );
    }
  });
}

QList<TodoList *> ReadTodoList::todoLists() const
{
    return objects();
}

} // namespace Queries
} // namespace DataBase
} // namespace OpenTodoList
