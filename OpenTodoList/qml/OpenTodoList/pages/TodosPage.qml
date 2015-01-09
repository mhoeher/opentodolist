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
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import QtQuick.Dialogs 1.2
import "../components" as Components
import "../style" as Style
import "../utils/DateUtils.js" as DateUtils
import net.rpdev.OpenTodoList.DataModel 1.0
import net.rpdev.OpenTodoList.DataBase 1.0
import net.rpdev.OpenTodoList.Models 1.0

Components.Page {
    id: todosPage

    property TodoList todoList: null

    signal todoSelected(Todo todo)

    title: todoList ? todoList.name : qsTr( "Todos" )
    width: 100
    height: 62

    Menu {
        id: contextMenu
        title: qsTr( "Todo" )

        MenuItem {
            text: qsTr( "Rename" )
            onTriggered: d.renaming = true
        }
        MenuItem {
            text: qsTr( "Delete" )
            onTriggered: confirmDeleteDialog.open()
        }
    }

    MessageDialog {
        id: confirmDeleteDialog
        title: qsTr( "Delete Todo" )
        text: qsTr( "Deleting the todo will remove " +
                   "it and any contained tasks. Are you " +
                   "sure you want to continue?" )
        standardButtons: StandardButton.Yes | StandardButton.Cancel
        onYes: dbConnection.disposeTodo(d.currentTodo)
    }

    DatabaseConnection {
        id: dbConnection
        database: application.database
    }

    QtObject {
        id: d

        property Todo currentTodo: null
        property bool renaming: false

        function showContextMenu( todo ) {
            currentTodo = todo;
            renaming = false;
            contextMenu.popup();
        }

    }

    TodoModel {
        id: todoModel
        database: application.database
        showDone: true
        todoList: todosPage.todoList
    }

    Component {
        id: viewDelegate

        Item {
            width: parent.width
            height: Math.max(
                        Math.max( label.height,
                                  edit.height,
                                  checkBox.height ) + Style.Measures.midSpace,
                        Style.Measures.optButtonHeight )

            Components.ItemBox {
                id: background
                anchors.fill: parent
            }

            MouseArea {
                anchors.fill: parent
                acceptedButtons: Qt.LeftButton | Qt.RightButton
                onClicked: {
                    switch ( mouse.button ) {
                    case Qt.LeftButton:
                        todosPage.todoSelected(display);
                        break;
                    case Qt.RightButton:
                        d.showContextMenu( display )
                        break;
                    default:
                        break;
                    }
                }
                onPressAndHold: {
                    d.showContextMenu( display )
                }
            }

            Style.H5 {
                id:  checkBox
                anchors {
                    left: parent.left
                    margins: Style.Measures.smallSpace
                    verticalCenter: parent.verticalCenter
                }
                font.family: Style.Fonts.symbols.name
                text: display.done ? Style.Symbols.checkedBox : Style.Symbols.uncheckedBox
            }

            MouseArea {
                anchors.centerIn: checkBox
                width: Style.Measures.optButtonHeight
                height: Style.Measures.optButtonHeight
                onClicked: display.done = !display.done
            }

            Style.P {
                id: label
                visible: !edit.visible
                text: display.title
                anchors {
                    left: checkBox.right
                    right: parent.right
                    verticalCenter: parent.verticalCenter
                    margins: Style.Measures.smallSpace
                }
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
            }

            TextField {
                id: edit
                text: display.title
                anchors {
                    left: checkBox.right
                    right: parent.right
                    verticalCenter: parent.verticalCenter
                    margins: Style.Measures.smallSpace
                }
                visible: d.currentTodo === display && d.renaming
                validator: RegExpValidator { regExp: /.+/ }

                onVisibleChanged: {
                    text = display.title;
                    focus = true;
                }
                onEditingFinished: {
                    d.renaming = false;
                    text = Qt.binding( function() { return display.title } );
                    focus = false;
                }
                onAccepted: {
                    display.title = edit.text;
                }
            }
        }
    }

    Component {
        id: headerDelegate
        Rectangle {
            id: header
            width: parent.width
            clip: true
            color: Style.Colors.primary
            z: 2
            height: Math.max( newTodoTitle.height,
                             plusButton.height ) +
                    Style.Measures.midSpace

            TextField {
                id: newTodoTitle
                placeholderText: qsTr( "Add Todo" )
                anchors {
                    left: parent.left
                    right: plusButton.left
                    verticalCenter: parent.verticalCenter
                    margins: Style.Measures.tinySpace
                }
                onAccepted: plusButtonMouseArea.clicked(null)
                onEditingFinished: focus = false
            }

            Style.H4 {
                id: plusButton
                font.family: Style.Fonts.symbols.name
                text: Style.Symbols.plus
                color: Style.Colors.lightText
                anchors {
                    right: parent.right
                    margins: Style.Measures.tinySpace
                    verticalCenter: parent.verticalCenter
                }
            }

            Component {
                id: newTodoComponent
                Todo {}
            }

            MouseArea {
                id: plusButtonMouseArea
                anchors.centerIn: plusButton
                width: Style.Measures.optButtonHeight
                height: Style.Measures.optButtonHeight
                onClicked: {
                    var newTitle = newTodoTitle.text;
                    var newTodoProperties = DateUtils.inputToTodoProperties(newTitle);
                    if ( todosPage.todoList && newTodoProperties.title !== "" ) {
                        var newTodo = newTodoComponent.createObject();
                        newTodo.title = newTodoProperties.title;
                        if ( newTodoProperties.dueDate ) {
                            newTodo.dueDate = newTodoProperties.dueDate;
                        }
                        newTodo.todoList = todosPage.todoList.uuid;
                        dbConnection.insertTodo( newTodo );
                        newTodo.destroy();
                        newTodoTitle.text = "";
                    }
                }
            }

            states: State {
                name: "hidden"
                when: todosPage.todoList === null
                PropertyChanges {
                    target: header
                    height: 0
                }
            }
        }
    }

    ScrollView {
        anchors.fill: parent

        ListView {
            id: listView
            model: todoModel
            delegate: viewDelegate
            header: headerDelegate
            headerPositioning: ListView.PullBackHeader
        }
    }

}

