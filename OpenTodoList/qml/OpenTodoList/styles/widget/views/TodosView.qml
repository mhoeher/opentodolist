/*
 *  OpenTodoList - A todo and task manager
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

import QtQuick 2.0
import QtQuick.Controls 1.0

import net.rpdev.OpenTodoList 1.0

import "../components"

import "../../../js/Utils.js" as Utils

GroupBox {
    id: view

    title: qsTr( "Todos" )

    property QtObject todoList: null
    property TodoSortFilterModel todosModel: TodoSortFilterModel {
        sourceModel: null
    }
    property alias currentTodo: todosTable.currentTodo

    function showTodoList( list ) {
        todoList = list;
        todosModel.sourceModel = list.entries;
        view.title = qsTr( "Todos in %1" ).arg( list.name );
        todosTable.reset();
    }

    TodoTableView {
        id: todosTable
        model: todosModel
    }
}
