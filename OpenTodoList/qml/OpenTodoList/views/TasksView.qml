/*
 *  OpenTodoList - A todo and task manager
 *  Copyright (C) 2015  Martin Höher <martin@rpdev.net>
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

import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Dialogs 1.2
import net.rpdev.OpenTodoList.DataModel 1.0
import net.rpdev.OpenTodoList.DataBase 1.0
import net.rpdev.OpenTodoList.Models 1.0
import "../style" as Style
import "../components" as Components

ListView {
    id: view

    property Todo todo

    model: taskModel
    delegate: taskDelegate
    header: taskHeaderDelegate
    headerPositioning: ListView.PullBackHeader
    clip: true

    TaskModel {
        id: taskModel
        database: application.database
        todo:  view.todo
    }

    DatabaseConnection {
        id: dbConnection
        database: application.database
    }

    Component {
        id: newTask
        Task {}
    }

    Component {
        id: taskDelegate

        Item {
            height: Math.max(
                        Math.max( checkBox.height,
                                  label.height,
                                  edit.height ) + Style.Measures.midSpace,
                        Style.Measures.optButtonHeight )
            width: parent.width
            opacity: display.done ? 0.5 : 1.0

            Components.ItemBox {
                anchors.fill: parent
            }
            Components.Symbol {
                id: checkBox
                font.family: Style.Fonts.symbols.name
                symbol: display.done ? Style.Symbols.checkedBox : Style.Symbols.uncheckedBox
                anchors {
                    left: parent.left
                    margins: Style.Measures.smallSpace
                    verticalCenter: parent.verticalCenter
                }
                onClicked: display.done = !display.done
            }
            Style.P {
                id: label
                text: display.title
                visible: !edit.visible
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                anchors {
                    left: checkBox.right
                    right: parent.right
                    margins: Style.Measures.smallSpace
                    verticalCenter: parent.verticalCenter
                }
            }
            MouseArea {
                anchors {
                    left: checkBox.right
                    top: parent.top
                    right: parent.right
                    bottom: parent.bottom
                }
                acceptedButtons: Qt.LeftButton | Qt.RightButton
                onPressAndHold: contextMenu.showPopupMenu(display)
                onClicked: {
                    switch ( mouse.button ) {
                    case Qt.LeftButton:
                        // TODO: Emit taskSelected signal
                        break;
                    case Qt.RightButton:
                        contextMenu.showPopupMenu(display);
                        break;
                    }
                }
            }
            TextField {
                id: edit
                visible: contextMenu.currentTask === display && contextMenu.renaming
                anchors {
                    left: checkBox.right
                    right: parent.right
                    margins: Style.Measures.smallSpace
                    verticalCenter: parent.verticalCenter
                }
                onVisibleChanged: {
                    if ( visible ) {
                        text = display.title;
                        focus = true;
                        forceActiveFocus();
                    }
                }
                onAccepted: {
                    display.title = text;
                }
                onEditingFinished: {
                    contextMenu.renaming = false;
                }
            }
        }
    }

    Component {
        id: taskHeaderDelegate
        Item {
            id: header
            z: 2

            function createTask() {
                if ( newTaskTitle.text !== "" &&
                        view.todo.isValid() ) {
                    var task = newTask.createObject();
                    task.title = newTaskTitle.text;
                    task.todo = view.todo.uuid;
                    dbConnection.insertTask( task );
                    task.destroy();
                    newTaskTitle.text = "";
                }
                newTaskTitle.focus = true;
                newTaskTitle.forceActiveFocus();
            }

            width: parent.width
            height: Math.max(newTaskTitle.height,
                             addTaskButton.height ) + Style.Measures.midSpace

            Components.ItemBox {
                anchors.fill: parent
                color: Style.Colors.primaryLight2
            }

            TextField {
                id: newTaskTitle
                validator: RegExpValidator { regExp: /.+/ }
                placeholderText: qsTr( "Add new task..." )
                anchors {
                    left: parent.left
                    right: addTaskButton.left
                    margins: Style.Measures.smallSpace
                    verticalCenter: parent.verticalCenter
                }
                onAccepted: header.createTask()
            }
            Components.Symbol {
                id: addTaskButton
                symbol: Style.Symbols.plus
                color: Style.Colors.lightText
                anchors {
                    right: parent.right
                    margins: Style.Measures.smallSpace
                    verticalCenter: parent.verticalCenter
                }
                onClicked: header.createTask()
            }
        }
    }

    Menu {
        id: contextMenu

        property Task currentTask: null
        property bool renaming: false

        function showPopupMenu(task) {
            currentTask = task;
            renaming = false;
            popup()
        }

        title: qsTr( "Task" )

        MenuItem {
            text: qsTr( "Rename" )
            onTriggered: contextMenu.renaming = true
        }
        MenuItem {
            text: qsTr( "Delete" )
            onTriggered: confirmDeleteDialog.open()
        }
    }

    MessageDialog {
        id: confirmDeleteDialog
        title: qsTr( "Delete Task" )
        text: qsTr( "Do you really want to delete the task?" )
        standardButtons: StandardButton.Yes | StandardButton.Cancel
        onYes: dbConnection.disposeTask(contextMenu.currentTask)
    }
}
