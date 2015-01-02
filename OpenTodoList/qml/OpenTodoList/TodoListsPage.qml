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
import net.rpdev.OpenTodoList.DataModel 1.0
import net.rpdev.OpenTodoList.Models 1.0
import net.rpdev.OpenTodoList.Theme 1.0
import net.rpdev.OpenTodoList.Components 1.0
import net.rpdev.OpenTodoList.Views 1.0

import net.rpdev.OpenTodoList.SystemIntegration 1.0

Page {
    id: page

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
                database: application.database
            }

            TodoListView {
                id: todoListView

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
            id: recentTodosTab

            name: qsTr( "Upcoming Todos" )

            Flickable {
                id: recentTodosTabFlickable
                anchors.fill: parent
                contentWidth: width
                contentHeight: schueduledTodosColumn.height
                clip: true

                Column {
                    id: schueduledTodosColumn
                    spacing: Measures.tinySpace
                    x: Measures.tinySpace
                    width: recentTodosTabFlickable.width - Measures.tinySpace * 2
                    height: childrenRect.height

                    TodoView {
                        id: dueTodayView

                        headerLabel: qsTr( "Due Today" )
                        height: contentItem.height + headerHeight
                        width: parent.width
                        clip: true
                        interactive: false
                        todos: TodoModel {
                            database: application.database
                            maxDueDate: new Date()
                        }

                        onTodoSelected: page.todoSelected( todo )
                    }

                    Item {
                        width: Measures.tinySpace
                        height: Measures.tinySpace
                    }

                    TodoView {
                        id: dueThisWeekView

                        headerLabel: qsTr( "Due this Week" )
                        height: contentItem.height + headerHeight
                        width: parent.width
                        clip: true
                        interactive: false
                        todos: TodoModel {
                            database: application.database
                            maxDueDate: {
                                var result = new Date( dueTodayView.todos.maxDueDate );
                                var date = result.getDate();
                                var offset = ( 6 - result.getDay() + Qt.locale().firstDayOfWeek ) % 7;
                                result.setDate( date + offset );
                                return result;
                            }
                            minDueDate: dueTodayView.todos.maxDueDate
                        }

                        onTodoSelected: page.todoSelected( todo )
                    }

                    Item {
                        width: Measures.tinySpace
                        height: Measures.tinySpace
                    }

                    TodoView {
                        id: scheduledLaterView

                        headerLabel: qsTr( "Scheduled for Later" )
                        height: contentItem.height + headerHeight
                        width: parent.width
                        clip: true
                        interactive: false
                        todos: TodoModel {
                            database: application.database
                            minDueDate: dueThisWeekView.todos.maxDueDate
                            limitCount: 10
                            backendSortMode: TodoModel.SortTodoByDueDate
                        }

                        onTodoSelected: page.todoSelected( todo )
                    }
                }
            }

            Timer {
                interval: 1000 * 60 * 60
                running: true
                repeat: true
                onTriggered: {
                    dueTodayView.todos.maxDueDate = new Date();
                }
            }
        }

        Tab {
            id: searchTab

            name: qsTr( "Search" )

            TodoView {
                id: searchView

                anchors.fill: parent
                todos: TodoModel {
                    database: application.database
                }

                onTodoSelected: page.todoSelected( todo )
            }
        }

        Tab {
            id: aboutApplicationTab
            name: qsTr("About")

            Column {
                anchors.fill: parent
                anchors.margins: Measures.midSpace
                spacing: Measures.midSpace

                Label {
                    text: qsTr( "OpenTodoList") + " " + application.applicationVersion
                    font.bold: true
                    width: parent.width
                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                }

                Label {
                    text: qsTr( "An Open Source todo and task management application.")
                    width: parent.width
                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                }

                Label {
                    text: qsTr( "Copyright 2013 - 2014, Martin Hoeher" )
                    width: parent.width
                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                }

                Label {
                    text: "<a href='http://www.rpdev.net/home/project/opentodolist'>http://www.rpdev.net/home/project/opentodolist</a>"
                    width: parent.width
                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                    onLinkActivated: Qt.openUrlExternally(link)
                }
            }
        }

        Tab {
            id: settingsTab
            name: qsTr( "Settings" )

            Button {
                anchors {
                    bottom: parent.bottom
                    right: parent.right
                    margins: Measures.tinySpace
                }
                text: qsTr( "Quit" )
                onClicked: application.handler.terminateApplication()
            }
        }
    }
}
