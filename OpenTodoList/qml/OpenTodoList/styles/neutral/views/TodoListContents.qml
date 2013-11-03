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
import "../controls"

Item {
    id: todoListContents

    clip: true

    signal todoSelected(QtObject todo)

    LinkLabel {
        id: todoListName

        label: todoListView.currentList ? todoListView.currentList.name : ""
        width: parent.width
        editable: true

        onEditFinished: todoListView.currentList.name = todoListName.label

        states: [
            State {
                name: "hidden"
                when: !todoListView.currentList
                PropertyChanges {
                    target: todoListName
                    height: 0
                }
            }
        ]

        transitions: [
            Transition {
                from: ""
                to: "hidden"
                reversible: true
                NumberAnimation {
                    properties: "height"
                    duration: 200
                }
            }
        ]
    }

    TodoView {
        width: todoListContents.width
        anchors.top: todoListName.bottom
        anchors.bottom: parent.bottom
        model: todoListView.model
        todoList: todoListView.currentList
        allowAddTodos: true

        onTodoSelected: todoListContents.todoSelected(todo)
    }
}
