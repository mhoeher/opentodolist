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
import net.rpdev.OpenTodoList.Core 1.0

Item {
    id: todoView

    property int spacing: 4
    property QtObject model: null
    property QtObject todoList: null
    property QtObject parentTodo: null
    property bool autoSize: false
    property int contentHeight: controls.height + listView.contentHeight + list.anchors.topMargin
    property bool allowAddTodos: false
    property bool allowFiltering: true
    property bool useSearchMode: false

    signal todoSelected(QtObject todo)

    clip: true
    width: 100
    height: autoSize ? contentHeight : 100

    onModelChanged: filterEdit.text = ""
    onParentTodoChanged: filterEdit.text = ""
    onTodoListChanged: filterEdit.text = ""

    Item {
        id: controls
        anchors { top: parent.top; left: parent.left; right: parent.right; margins: 10 }
        height: childrenRect.height

        Column {
            spacing: 10

            SimpleTextInput {
                id: filterEdit
                placeholderText: todoView.useSearchMode ? qsTr( "Search todos..." ) : qsTr( "Filter todos..." )
                text: ""
                width: controls.width
                visible: todoView.allowFiltering
            }

            Item {
                id: addTodoControl
                visible: todoView.allowAddTodos && ( todoView.todoList || todoView.parentTodo )
                width: controls.width
                height: childrenRect.height

                SimpleTextInput {
                    id: newTodoTitle
                    text: ""
                    placeholderText: qsTr( "Add new todo..." )
                    anchors {
                        left: parent.left
                        right: addNewTodoButton.left
                    }
                    onApply: addNewTodoButton.createTodo()
                }

                SymbolButton {
                    id: addNewTodoButton
                    text: "+"
                    anchors {
                        right: parent.right
                    }

                    onClicked: createTodo()

                    function createTodo() {
                        if ( newTodoTitle.text !== "" ) {
                            var todoList;
                            if ( todoView.parentTodo ) {
                                todoList = todoView.parentTodo.todoList;
                            } else {
                                todoList = todoView.todoList;
                            }
                            var todo = todoList.addTodo();
                            todo.title = newTodoTitle.text;
                            if ( todoView.parentTodo ) {
                                todo.parentTodo = todoView.parentTodo;
                            }
                            newTodoTitle.text = "";
                        }
                    }
                }
            }
        }

        Behavior on height { SmoothedAnimation { duration: 500 } }
    }

    Item {
        id: list
        anchors { top: controls.bottom; topMargin: layout.minimumButtonHeight / 2; left: parent.left; right: parent.right; bottom: parent.bottom }
        clip: true

        ListView {
            id: listView
            anchors.fill: parent
            interactive: !todoView.autoSize
            model: TodoModel {
                library: window.library
                //sourceModel: !todoView.useSearchMode || filterEdit.text !== "" ? todoView.model : null
                //sortMode: TodoSortFilterModel.PrioritySort
                //searchString: filterEdit.text
            }

            delegate: TodoListEntry {
                todo: object
                width: listView.width
                hasNext: todoView.model && index !== todoView.model.count - 1
                onClicked: {
                    todoView.todoSelected( object )
                }
            }
        }
    }
}

