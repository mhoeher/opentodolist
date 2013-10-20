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

import QtQuick 2.1
import QtQuick.Window 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0

import net.rpdev.OpenTodoList 1.0

import "views"
import "dialogs"

ApplicationWindow {
    id: window
    width: 640
    height: 480
    visible: true

    property Settings settings : Settings {
        groups: [ "ApplicationViewer", "widgets" ]
    }

    Component.onCompleted: {
        width = settings.getValue( "width", width );
        height = settings.getValue( "height", height );
        x = settings.getValue( "x", x );
        y = settings.getValue( "y", y );
        show();
        applicationViewer.onBeforeReload.connect( close );
    }

    onClosing: {
        settings.setValue( "width", width );
        settings.setValue( "height", height );
        settings.setValue( "x", x );
        settings.setValue( "y", y );
    }

    menuBar: MenuBar {
        Menu {
            title: qsTr( "Todo List" )

            MenuItem {
                text: qsTr( "New List" )
                shortcut: "Ctrl+N"
                onTriggered: {
                    var component = Qt.createComponent( "dialogs/NewTodoListDialog.qml" );
                    var dialog = component.createObject( window );
                    dialog.exec();
                }
            }

            MenuSeparator {}

            MenuItem {
                text: qsTr( "Quit" )
                shortcut: "Ctrl+Q"
                onTriggered: Qt.quit()
            }
        }

        Menu {
            title: qsTr( "Settings" )

            MenuItem {
                text: qsTr( "Style..." )
                onTriggered: {
                    var c = Qt.createComponent( "dialogs/StyleSelectionDialog.qml" );
                    var d = c.createObject( window );
                    d.exec();
                }
            }
        }

        Menu {
            title: qsTr( "Help" )

            MenuItem {
                text: qsTr( "About" )
            }
        }
    }

    SplitView {
        anchors.fill: parent

        TodoListsView {
            id: todoListsView
            width: 300

            onTodoListChanged: {
                todosView.showTodoList( todoListsView.todoList );
            }
        }

        SplitView {
            id: todosPane

            orientation: Qt.Vertical

            TodosView {
                id: todosView
                Layout.fillHeight: true
            }

            TodoDetailsView {
                id: todoDetailsView

                property int lastHeight: todosPane.height / 2

                todo: todosView.currentTodo
                height: 0
                width: 500
                Layout.minimumHeight: 0

                onTodoChanged: {
                    if ( todo ) {
                        showTodoDetails.start();
                    } else {
                        lastHeight = height;
                        hideTodoDetails.start();
                    }
                }

                NumberAnimation {
                    id: showTodoDetails
                    target: todoDetailsView
                    properties: "height"
                    to: todoDetailsView.lastHeight
                    duration: 200
                }

                NumberAnimation {
                    id: hideTodoDetails
                    target: todoDetailsView
                    properties: "height"
                    to: 0
                    duration: 200
                }
            }
        }
    }

}

