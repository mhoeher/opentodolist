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
import net.rpdev.OpenTodoList.Components 1.0
import net.rpdev.OpenTodoList.Views 1.0

Page {
    id: page

    property TodoListLibrary library

    signal todoSelected( Todo todo )

    name: qsTr( "Todo Lists" )

    TabView {
        id: tabView
        anchors.fill: parent
        Tab {
            id: todoListTab

            name: qsTr( "Todo Lists" )

            TodoModel {
                id: todoModel
                library: page.library
                queryType: TodoModel.QueryTopLevelTodosInTodoList
            }

            TodoListView {
                id: todoListView

                library: page.library
                showTodosInline: todoListTab.width < Measures.minimumPageWidth * 2
                highlightCurrentTodoList: !showTodosInline
                anchors {
                    left: parent.left
                    right: todoView.left
                    top: parent.top
                    bottom: parent.bottom
                }

                onTodoListSelected: {
                    todoModel.todoList = todoList;
                }

                onTodoSelected: page.todoSelected( todo )
            }

            TodoView {
                id: todoView

                anchors {
                    right: parent.right
                    top: parent.top
                    bottom: parent.bottom
                }
                width: todoModel.todoList !== null && parent.width > 2 * Measures.minimumPageWidth ?
                           parent.width - Measures.minimumPageWidth : 0
                todos: todoModel
                backgroundVisible: true
                clip: true

                onTodoSelected: page.todoSelected( todo )

                Behavior on width { SmoothedAnimation { velocity: 1500 } }
            }
        }
        Tab {
            id: dueTodayTab

            name: qsTr( "Due Today" )

            TodoView {
                id: dueTodayView

                anchors.fill: parent
                todos: TodoModel {
                    library: page.library
                    queryType: TodoModel.QueryFilterTodos
                    maxDueDate: new Date()
                }

                onTodoSelected: page.todoSelected( todo )
            }
            Timer {
                interval: 1000 * 60 * 60
                running: true
                repeat: true
                onTriggered: dueTodayView.todos.maxDueDate = new Date()
            }
        }
        Tab {
            id: dueThisWeekTab

            name: qsTr( "Due This Week" )

            TodoView {
                id: dueThisWeekView

                function lastDayThisWeek() {
                    var result = new Date();
                    result.setDate( result.getDate() - result.getDay() + Qt.locale().firstDayOfWeek + 6 );
                    return result;
                }

                anchors.fill: parent
                todos: TodoModel {
                    library: page.library
                    queryType: TodoModel.QueryFilterTodos
                    maxDueDate: dueThisWeekView.lastDayThisWeek()
                }

                onTodoSelected: page.todoSelected( todo )
            }
            Timer {
                interval: 1000 * 60 * 60
                running: true
                repeat: true
                onTriggered: dueThisWeekView.todos.maxDueDate = dueThisWeekView.lastDayThisWeek()
            }
        }
        Tab {
            id: searchTab

            name: qsTr( "Search" )

            TodoView {
                id: searchView

                anchors.fill: parent
                todos: TodoModel {
                    library: page.library
                    queryType: TodoModel.QuerySearchTodos
                }

                onTodoSelected: page.todoSelected( todo )
            }
        }
        Tab {
            id: trashTab

            name: qsTr( "Trash" )

            TodoView {
                id: trashView

                anchors {
                    left: parent.left
                    right: parent.right
                    top: parent.top
                    bottom: emptyTrashButton.top
                    bottomMargin: Measures.tinySpace
                }

                todos: TodoModel {
                    id: trashModel
                    library: page.library
                    queryType: TodoModel.QueryFilterTodos
                    showDeleted: true
                    hideUndeleted: true
                }

                onTodoSelected: page.todoSelected( todo )
            }
            Button {
                id: emptyTrashButton
                text: qsTr( "Empty Trash" )
                anchors {
                    right: parent.right
                    bottom: parent.bottom
                    margins: Measures.tinySpace
                }
                onClicked: {
                    var dialog = confirmEmptyTrashDialog.createObject( this );
                    dialog.show();
                }
            }

            Component {
                id: confirmEmptyTrashDialog

                Overlay {
                    Rectangle {
                        color: Colors.window
                        border {
                            width: Measures.smallBorderWidth
                            color: Colors.border
                        }
                        anchors {
                            left: parent.left
                            right: parent.right
                            verticalCenter: parent.verticalCenter
                            margins: Measures.tinySpace
                        }
                        height: childrenRect.height + Measures.tinySpace * 2

                        Column {
                            anchors {
                                left: parent.left
                                right: parent.right
                                top: parent.top
                                margins: Measures.tinySpace
                            }
                            spacing: Measures.tinySpace

                            Label {
                                text: qsTr( "Empty Trash?" )
                                width: parent.width
                                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                                font.bold: true
                            }

                            Label {
                                text: qsTr( "You cannot undo this operation. Do you want to continue?" )
                                anchors {
                                    left: parent.left
                                    right: parent.right
                                    margins: Measures.tinySpace
                                }
                                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                            }

                            Item {
                                width: parent.width
                                height: childrenRect.height

                                Button {
                                    id: continueEmptyTrashButton
                                    text: qsTr( "Cancel" )
                                    anchors {
                                        right: parent.right
                                    }

                                    onClicked: close()
                                }
                                Button {
                                    text: qsTr( "Continue" )
                                    anchors {
                                        right: continueEmptyTrashButton.left
                                        margins: Measures.tinySpace
                                    }

                                    onClicked: {
                                        var todos = [];
                                        for ( var i = 0; i < trashModel.count; ++i ) {
                                            todos.push( trashModel.get( i ) );
                                        }
                                        for ( var i = 0; i < todos.length; ++i ) {
                                            todos[ i ].dispose();
                                        }
                                        close();
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
