/*
 *  OpenTodoList - A todo and task manager
 *  Copyright (C) 2014  Martin Höher <martin@rpdev.net>
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
import net.rpdev.OpenTodoList.Theme 1.0
import net.rpdev.OpenTodoList.Views 1.0
import net.rpdev.OpenTodoList.Components 1.0

Item {
    id: todoView

    property TodoModel todos
    property alias currentIndex: view.currentIndex
    property alias color: background.color
    property alias backgroundVisible: background.visible
    property alias contentItem: view.contentItem

    signal todoSelected( Todo todo )

    QtObject {
        id: d

        property bool canCreateTodos: {
            if ( todos ) {
                switch ( todos.queryType ) {
                case TodoModel.QueryTopLevelTodosInTodoList:
                    return todos.todoList && todos.todoList.canCreateTodos;
                case TodoModel.QuerySubTodosOfTodo:
                    return todos.parentTodo && todos.parentTodo.canCreateTodos;
                default: return false;
                }
            }
            return false;
        }

        function createTodo( title ) {
            if ( todos ) {
                switch ( todos.queryType ) {
                case TodoModel.QueryTopLevelTodosInTodoList:
                    todos.todoList.addTodo( title );
                    break;
                case TodoModel.QuerySubTodosOfTodo:
                    todos.parentTodo.addTodo( title );
                    break;
                default:
                    break;
                }
            }
        }
    }

    Rectangle {
        id: background
        anchors.fill: parent
        color: Colors.listItem
        visible: false
    }

    AnimatedListView {
        id: view

        model: todoView.todos
        currentIndex: -1
        highlightMoveDuration: 200
        anchors.fill: parent

        Component {
            id: headerComponent
            Item {
                width: view.width
                height: childrenRect.height + Measures.tinySpace * 2

                function add() {
                    if ( newTodoTitle.text !== "" ) {
                        d.createTodo( newTodoTitle.text );
                        newTodoTitle.text = "";
                    }
                }

                Item {
                    id: newTodoInputPane
                    width: parent.width
                    height: d.canCreateTodos ? hildrenRect.height : 0
                    anchors.top: parent.top
                    visible: height > 0

                    SingleLineTextInput {
                        id: newTodoTitle

                        anchors {
                            left: parent.left
                            right: addTodoButton.left
                            top: parent.top
                            margins: Measures.tinySpace
                        }
                        placeholder: qsTr( "Type to add new todo" )

                        onAccept: add()
                    }
                    SymbolButton {
                        id: addTodoButton

                        symbol: Symbols.plus
                        anchors {
                            right: parent.right
                            top: parent.top
                            margins: Measures.tinySpace
                        }

                        onClicked: add()
                    }
                }

                Item {
                    id: filterTodoPane
                    width: parent.width
                    height: childrenRect.height
                    anchors.top: newTodoInputPane.bottom
                    anchors.margins: Measures.tinySpace

                    SingleLineTextInput {
                        anchors {
                            left: parent.left
                            right: showDone.left
                            top: parent.top
                            margins: Measures.tinySpace
                        }
                        placeholder: qsTr( "Type to filter todos..." )
                        onTextChanged: {
                            if ( todoView.todos ) {
                                todoView.todos.filter = text;
                            }
                        }
                    }

                    SymbolButton {
                        id: showDone
                        anchors {
                            right: showDeleted.left
                            top: parent.top
                            margins: Measures.tinySpace
                        }
                        symbol: Symbols.checkedBox
                        checked: todoView.todos && todoView.todos.showDone
                        onClicked: {
                            if ( todoView.todos ) {
                                todoView.todos.showDone = !todoView.todos.showDone
                            }
                        }
                    }

                    SymbolButton {
                        id: showDeleted
                        anchors {
                            right: parent.right
                            top: parent.top
                            margins: Measures.tinySpace
                        }
                        symbol: Symbols.thrash
                        checked: todoView.todos && todoView.todos.showDeleted
                        onClicked: {
                            if ( todoView.todos ) {
                                todoView.todos.showDeleted = !todoView.todos.showDeleted
                            }
                        }
                    }
                }
            }
        }

        delegate: TodoViewDelegate {
            todo: display
            onClicked: {
                currentIndex = index;
                todoView.todoSelected( display );
            }
        }
        header: headerComponent
    }
}
