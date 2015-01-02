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
import net.rpdev.OpenTodoList.Models 1.0
import net.rpdev.OpenTodoList.DataBase 1.0
import net.rpdev.OpenTodoList.Components 1.0
import net.rpdev.OpenTodoList.Theme 1.0
import net.rpdev.OpenTodoList.Views 1.0

AnimatedListView {
    id: todoListView

    property TodoList currentTodoList
    property bool showTodosInline: false
    property bool highlightCurrentTodoList: false

    signal todoListSelected( TodoList todoList )
    signal todoSelected( Todo todo )

    highlightFollowsCurrentItem: true
    highlightMoveDuration: 200
    highlightResizeDuration: 200
    currentIndex: -1

    model: todoListModel

    delegate: TodoListViewDelegate {
        todoList: display
        maxHeight: todoListView.height
        drawDivider: index < todoListModel.count - 1
        useHighlight: highlightCurrentTodoList && currentIndex == index
        anchors.leftMargin: Measures.tinySpace
        onClicked: {
            currentIndex = index;
            currentTodoList = display;
            if ( showTodos ) {
                showTodos = false;
            } else {
                todoListSelected( display );
                showTodos = Qt.binding( function() { return showTodosInline && currentIndex == index; } );
            }
        }
        onTodoSelected: todoListView.todoSelected( todo )
    }

    highlight: Item {
        width: parent.width
        Rectangle {
            id: highlightBar
            anchors {
                left: parent.left
                top: parent.top
                bottom: parent.bottom
            }
            width: Measures.tinySpace
            gradient: Gradient {
                GradientStop {
                    color: Colors.secondary1Light1
                    position: 0.0
                }
                GradientStop {
                    color: Colors.secondary1
                    position: 0.5
                }
                GradientStop {
                    color: Colors.secondary1Light1
                    position: 1.0
                }
            }
        }
    }

    header: Item {
        id: header

        width: parent.width
        height: childrenRect.height + Measures.tinySpace * 2
        visible: height > 0

        QtObject {
            id: d

            function writableBackends() {
                var writableBackends = [];
                var backends = library.backends;
                for ( var i = 0; i < backends.length; ++i ) {
                    var backend = backends[i];
                    if ( backend.canAddTodoList() ) {
                        writableBackends.push( backend );
                    }
                }
                return writableBackends;
            }

            function addTodoList() {
                if ( newTodoListNameEdit.text !== "" ) {
                    var dialog = accountSelectionDialog.createObject( todoListView );
                    dialog.newListName = newTodoListNameEdit.text;
                    dialog.onAccept.connect( function() {
                        dialog.todoList.name = dialog.newListName;
                        dialog.todoList.account = dialog.selectedAccount.uuid;
                        dialog.dbConnection.database = application.database;
                        dialog.dbConnection.insertTodoList( dialog.todoList );
                    } );
                    dialog.show();
                    newTodoListNameEdit.text = "";
                }
            }

            property Component dialog: Component {
                id: accountSelectionDialog

                Overlay {
                    id: overlay

                    property Account selectedAccount: null
                    property string newListName
                    property TodoList todoList : TodoList {}
                    property DatabaseConnection dbConnection : DatabaseConnection {
                        database: application.database
                    }

                    Keys.onPressed: {
                        if ( ( event.key === Qt.Key_Enter || event.key === Qt.Key_Return ) &&
                                selectedAccount ) {
                            accept();
                            close();
                        }
                    }

                    signal accept()

                    Rectangle {
                        anchors.fill: parent
                        anchors.margins: Measures.tinySpace
                        color: Colors.window
                        border {
                            width: Measures.smallBorderWidth
                            color: Colors.border
                        }

                        Label {
                            id: inputPromtLabel
                            anchors {
                                left: parent.left
                                right: parent.right
                                top: parent.top
                                margins: Measures.tinySpace
                            }
                            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                            text: qsTr( "Please select in which account to create the new todo list:" )
                        }

                        ListView {
                            id: accountView
                            model: AccountModel {
                                database: application.database
                            }

                            anchors {
                                top: inputPromtLabel.bottom
                                left: parent.left
                                right: parent.right
                                bottom: cancelButton.top
                                margins: Measures.tinySpace
                            }
                            highlightFollowsCurrentItem: true
                            currentIndex: -1

                            highlight: Item {
                                Rectangle {
                                    width: overlay.selectedAccount ? Measures.tinySpace : 0
                                    height: parent.height - Measures.tinySpace
                                    y: Measures.tinySpace
                                    color: Colors.secondary1
                                }
                            }

                            delegate: Item {
                                width: parent.width
                                height: childrenRect.height + Measures.tinySpace

                                MouseArea {
                                    anchors.fill: itemBackground
                                    onClicked: {
                                        overlay.selectedAccount = display
                                        accountView.currentIndex = index
                                    }

                                }

                                Rectangle {
                                    id: itemBackground
                                    anchors {
                                        left: parent.left
                                        right: parent.right
                                        top: parent.top
                                        margins: Measures.tinySpace
                                    }

                                    height: childrenRect.height + Measures.smallSpace * 2
                                    color: overlay.selectedAccount === display ? Colors.listItem : Colors.window

                                    Label {
                                        id: accountNameLabel
                                        anchors {
                                            left: parent.left
                                            right: parent.right
                                            top: parent.top
                                            margins: Measures.tinySpace
                                            topMargin: Measures.smallSpace
                                        }
                                        wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                                        text: display.name
                                        font.bold: true
                                    }
                                }
                            }
                        }

                        Button {
                            id: okButton
                            text: qsTr( "Create" )
                            enabled: overlay.selectedAccount
                            anchors {
                                bottom: parent.bottom
                                right: parent.right
                                margins: Measures.tinySpace
                            }
                            onClicked: {
                                overlay.accept( overlay.selectedAccount );
                                overlay.close();
                            }
                        }
                        Button {
                            id: cancelButton
                            text: qsTr( "Cancel" )
                            anchors {
                                bottom: parent.bottom
                                right: okButton.left
                                margins: Measures.tinySpace
                            }
                            onClicked: overlay.close()
                        }
                    }
                }
            }
        }

        SingleLineTextInput {
            id: newTodoListNameEdit
            anchors {
                left: parent.left
                top: parent.top
                right: addNewTodoListButton.left
                margins: Measures.tinySpace
            }
            placeholder: qsTr( "Add new list..." )

            onAccept: d.addTodoList()
        }
        SymbolButton {
            id: addNewTodoListButton
            anchors {
                right: parent.right
                top: parent.top
                margins: Measures.tinySpace
            }
            symbol: Symbols.plus

            onClicked: d.addTodoList()
        }

        states: State {
            name: "hidden"
            when: d.writableBackends().length === 0
            PropertyChanges {
                target: header
                height: 0
            }
        }
    }

    TodoListModel {
        id: todoListModel
        database: application.database
    }
}
