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
import net.rpdev.OpenTodoList 1.0

View {
    id: todoListView
    
    property QtObject currentList : null
    property TodoSortFilterModel model : TodoSortFilterModel {
        sourceModel: currentList ? currentList.entries : library.todos
        searchString: filterText.text
    }

    signal todoSelected(QtObject todo)
    signal showTrashForList( QtObject list )

    onCurrentListChanged: filterText.text = ""
    
    toolButtons: [
        ToolButton {
            label: "New List"

            onClicked: newTodoListView.hidden = false
        },
        ToolButton {
            label: "Quit"

            onClicked: Qt.quit()
        },
        ToolButton {
            label: "Trash Bin"
            enabled: currentList !== null

            onClicked: if ( todoListView.currentList ) {
                           todoListView.showTrashForList( todoListView.currentList );
                       }
        }

    ]
    
    Item {
        id: sideBar

        width: todoListView.currentList || true ? 200 : todoListView.clientWidth
        height: todoListView.clientHeight
        
        Behavior on width { SmoothedAnimation { velocity: 1200 } }
        
        Button {
            id: showAllTodosButton
            label: "All Todos"
            down: todoListView.currentList == null
            width: parent.width

            onClicked: todoListView.currentList = null
        }

        ListView {
            width: parent.width
            anchors.top: showAllTodosButton.bottom
            anchors.topMargin: 4
            anchors.bottom: parent.bottom
            model: library.todoLists
            spacing: 4
            delegate: Button {
                width: parent.width
                label: object.name
                down: todoListView.currentList == object

                onClicked: todoListView.currentList = object
            }
        }
        
    }
    
    Item {
        id: todoListContents
        anchors.left: sideBar.right
        anchors.leftMargin: 10
        width: todoListView.clientWidth - 200 - 10
        Column {
            spacing: 5
            
            Item {
                height: todoListView.currentList ? childrenRect.height : 0
                width: todoListContents.width
                SimpleTextInput {
                    id: newTodoTitle
                    anchors { left: parent.left; right: parent.right; rightMargin: addNewTodoButton.width + 10 }
                    text: ""

                    onApply: addNewTodoButton.createNewTodo()
                }
                Button {
                    id: addNewTodoButton
                    label: "Add"
                    
                    anchors.right: parent.right
                    
                    onClicked: createNewTodo()

                    function createNewTodo() {
                        var todo = todoListView.currentList.addTodo();
                        todo.title = newTodoTitle.text;
                        newTodoTitle.text = "";
                    }
                }

                Behavior on height { SmoothedAnimation { velocity: 1200 } }
            }

            Item {
                height: childrenRect.height
                width: todoListContents.width
                SimpleTextInput {
                    id: filterText
                    anchors { left: parent.left; right: parent.right }
                    text: ""
                }
            }
            
            ListView {
                model: todoListView.model
                width: todoListContents.width
                height: todoListView.clientHeight
                clip: true
                spacing: 4
                delegate: TodoListEntry {
                    todo: object
                    onClicked: {
                        todoListView.todoSelected( object )
                    }
                }
            }
        }
    }
    
}
