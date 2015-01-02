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

import QtQuick 2.0
import net.rpdev.OpenTodoList.DataModel 1.0
import net.rpdev.OpenTodoList.DataBase 1.0
import net.rpdev.OpenTodoList.Models 1.0
import net.rpdev.OpenTodoList.Theme 1.0
import net.rpdev.OpenTodoList.Views 1.0
import net.rpdev.OpenTodoList.Components 1.0

Item {
    id: taskView

    property TaskModel tasks
    property alias currentIndex: view.currentIndex
    property alias color: background.color
    property alias backgroundVisible: background.visible
    property alias contentItem: view.contentItem
    property bool trashView: false
    property alias interactive: view.interactive

    QtObject {
        id: d

        property Component newTaskHelperComponent : Component {
            id: newTaskHelper

            Item {
                property Task task : Task {}
                property DatabaseConnection dbConnection : DatabaseConnection {
                    database: application.database
                }
            }
        }

        function addTask( title ) {
            if ( title !== "" && tasks.todo ) {
                var helper = newTaskHelperComponent.createObject( taskView );
                helper.task.title = title;
                helper.task.todo = tasks.todo.uuid;
                helper.dbConnection.insertTask( helper.task );
                helper.destroy();
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

        model: taskView.tasks
        currentIndex: -1
        highlightMoveDuration: 200
        clip: true
        anchors.fill: parent

        header: Item {
            width: view.width
            height: childrenRect.height + Measures.tinySpace

            SingleLineTextInput {
                id: newTaskTitle
                anchors {
                    top: parent.top
                    left: parent.left
                    right: addTaskButton.left
                    margins: Measures.tinySpace
                }
                placeholder: qsTr( "Add new task..." )
                onAccept: {
                    d.addTask( newTaskTitle.text );
                    newTaskTitle.text = "";
                }
            }
            SymbolButton {
                id: addTaskButton
                symbol: Symbols.plus
                anchors {
                    top: parent.top
                    right: parent.right
                    margins: Measures.tinySpace
                }
                onClicked: {
                    d.addTask( newTaskTitle.text );
                    newTaskTitle.text = "";
                }
            }
        }

        delegate: TaskViewDelegate {
            task: display
        }
    }
}
