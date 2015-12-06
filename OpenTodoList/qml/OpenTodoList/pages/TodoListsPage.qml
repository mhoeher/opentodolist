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
import QtQuick.Dialogs 1.2
import "../components" as Components
import "../style" as Style
import net.rpdev.OpenTodoList.DataModel 1.0
import net.rpdev.OpenTodoList.DataBase 1.0
import net.rpdev.OpenTodoList.Models 1.0

Components.Page {
    id: todoListsPage
    title: qsTr( "Todo Lists" )

    signal todoListSelected( TodoList todoList )

    DatabaseConnection {
        id: dbConnection
        database: application.database
    }

    Menu {
        id: todoListContextMenu
        title: qsTr( "Todo List" )

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
        title: qsTr( "Delete Todo List" )
        text: qsTr( "This will delete the todo list and all contained " +
                    "todos. Are you sure you want to proceed?" )
        standardButtons: StandardButton.Yes | StandardButton.No
        onYes: {
            console.debug( "Releasing todo list" )
            dbConnection.disposeTodoList( d.currentTodoList )
        }
    }

    QtObject {
        id: d

        property TodoList currentTodoList
        property bool renaming: false

        function showContextMenu( todoList ) {
            renaming = false
            currentTodoList = todoList;
            todoListContextMenu.popup();
        }
    }

    TodoListModel {
        id: todoLists
        database: application.database
    }

    Component {
        id: viewDelegate

        Item {
            width: parent.width
            height: Math.max(
                        Math.max(
                            label.height,
                            edit.height ) + Style.Measures.midSpace,
                        Style.Measures.optButtonHeight )

            Components.ItemBox {
                id: background
                anchors.fill: parent
            }

            Style.P {
                id: label
                text: display.name
                y: Style.Measures.midSpace
                anchors {
                    left: parent.left
                    right: parent.right
                    margins: Style.Measures.midSpace
                    verticalCenter: parent.verticalCenter
                }
                visible: !edit.visible
            }

            MouseArea {
                anchors.fill: parent
                acceptedButtons: Qt.LeftButton | Qt.RightButton
                onClicked: {
                    d.currentTodoList = null;
                    switch ( mouse.button ) {
                    case Qt.LeftButton:
                        todoListsPage.todoListSelected(display)
                        break;
                    case Qt.RightButton:
                        d.showContextMenu(display);
                        break;
                    default:
                        break;
                    }
                }
                onPressAndHold: d.showContextMenu(display)
            }
            TextField {
                id: edit
                visible: d.currentTodoList === display && d.renaming
                y: Style.Measures.midSpace
                anchors {
                    left: parent.left
                    right: parent.right
                    margins: Style.Measures.midSpace
                    verticalCenter: parent.verticalCenter
                }
                validator: RegExpValidator { regExp: /.+/ }
                onVisibleChanged: {
                    if ( visible ) {
                        edit.text = display.text;
                        edit.focus = true;
                        edit.forceActiveFocus();
                    }
                }
                onAccepted: {
                    display.name = displayText;
                    text = Qt.binding( function() { return display.name; } );
                    d.renaming = false;
                }
                onEditingFinished: d.renaming = false
            }
        }
    }

    Component {
        id: headerDelegate

        Rectangle {
            id: header
            height: childrenRect.height + Style.Measures.midSpace
            width: parent.width
            color: Style.Colors.primary
            z: 2
            clip: true

            TextField {
                id: newTodoListTitle
                placeholderText: qsTr( "Add Todo List" )
                anchors {
                    left: parent.left
                    top: parent.top
                    right: createTodoListButton.left
                    margins: Style.Measures.tinySpace
                }
                onAccepted: {
                    if ( createTodoListButton.enabled ) {
                        createTodoListButton.clicked()
                    }
                }
            }

            Style.H6 {
                id: accountSelectionLabel
                text: qsTr( "Select an Account:" )
                color: Style.Colors.lightText
                anchors {
                    left: parent.left
                    right: parent.right
                    top: newTodoListTitle.bottom
                    margins: Style.Measures.tinySpace
                }
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
            }

            ComboBox {
                id: newTodoListAccount
                model: AccountModel {
                    database: application.database
                }
                textRole: "text"
                anchors {
                    left: parent.left
                    right: createTodoListButton.left
                    top: accountSelectionLabel.bottom
                    margins: Style.Measures.tinySpace
                }
            }

            Button {
                id: createTodoListButton
                text: qsTr( "Add" )
                enabled: newTodoListTitle.text !== "" &&
                         newTodoListAccount.currentIndex >= 0
                anchors {
                    right: parent.right
                    bottom: newTodoListAccount.bottom
                }

                onClicked: {
                    var helper = createTodoListHelper.createObject();
                    helper.todoList.name = newTodoListTitle.displayText;
                    helper.todoList.account = newTodoListAccount.model.objects[
                            newTodoListAccount.currentIndex].uuid;
                    helper.dbConnection.insertTodoList(helper.todoList);
                    helper.destroy();
                    newTodoListTitle.text = "";
                    newTodoListTitle.focus = true;
                    newTodoListTitle.forceActiveFocus();
                }
            }

            Component {
                id: createTodoListHelper
                Item {
                    property DatabaseConnection dbConnection: DatabaseConnection {
                        database: application.database
                    }
                    property TodoList todoList : TodoList {
                    }
                }
            }

            onStateChanged: if ( state == "minimizedState" ) {
                                listView.positionViewAtBeginning();
                            }

            states: State {
                name: "minimizedState"
                when: newTodoListTitle.text === ""
                PropertyChanges {
                    target: header
                    height: newTodoListTitle.y + newTodoListTitle.height +
                            Style.Measures.tinySpace
                }
            }

            Behavior on height { SmoothedAnimation { duration: 200 } }

        }
    }

    ScrollView {
        anchors {
            fill: parent
        }
        ListView {
            anchors {
                fill: parent
            }
            id: listView
            clip: true
            model: todoLists
            delegate: viewDelegate
            header: headerDelegate
            headerPositioning: ListView.PullBackHeader
        }
    }
}

