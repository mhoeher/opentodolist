/*
 *  OpenTodoListDesktopQml - Desktop QML frontend for OpenTodoList
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

    property alias filterText: filterText.text

    signal todoSelected(QtObject todo)

    Column {
        id: controlsColumns
        spacing: 5
        height: childrenRect.height

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

        Item {
            height: todoListView.currentList ? Math.max( newTodoTitle.height, addNewTodoButton.height ) : 0
            width: todoListContents.width
            SimpleTextInput {
                id: newTodoTitle
                anchors { left: parent.left; right: parent.right; rightMargin: addNewTodoButton.width + 10 }
                text: ""
                placeholderText: "Add new todo"

                onApply: addNewTodoButton.createNewTodo()
            }
            Button {
                id: addNewTodoButton
                label: "\uf067"
                font.family: symbolFont.name

                anchors.right: parent.right

                onClicked: createNewTodo()

                function createNewTodo() {
                    if ( newTodoTitle.text != "" ) {
                        var todo = todoListView.currentList.addTodo();
                        todo.title = newTodoTitle.text;
                        newTodoTitle.text = "";
                    }
                }
            }
            Behavior on height { SmoothedAnimation { velocity: 120 } }
        }

        Item {
            id: filterItem
            height: childrenRect.height
            width: todoListContents.width
            SimpleTextInput {
                id: filterText
                anchors { left: parent.left; right: parent.right }
                text: ""
                placeholderText: "Filter todos"

                onTextChanged: todoListView.filterText = text
            }
        }
    }

    TodoView {
        width: todoListContents.width
        anchors.top: controlsColumns.bottom
        anchors.bottom: parent.bottom
        model: todoListView.model

        onTodoSelected: todoListContents.todoSelected(todo)
    }
}
