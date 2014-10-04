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
import net.rpdev.OpenTodoList.Components 1.0
import net.rpdev.OpenTodoList.Theme 1.0
import net.rpdev.OpenTodoList.Views 1.0

FocusScope {
    id: delegate

    property TodoList todoList: null
    property bool showTodos: false
    property int maxHeight: 0
    property bool drawDivider: false
    property alias highlightColor: background.color
    property bool useHighlight: false

    signal clicked()
    signal todoSelected( Todo todo )

    anchors { left: parent.left; right: parent.right }
    height: showTodos ? maxHeight : titlePane.height + Measures.smallSpace
    clip: showTodos

    onShowTodosChanged: {
        if ( showTodos ) {
            var model = Qt.createQmlObject( "import net.rpdev.OpenTodoList.Core 1.0; TodoModel {}", this );
            model.todoList = todoList;
            model.queryType = TodoModel.QueryTopLevelTodosInTodoList;
            todosView.todos = model;
        }
    }

    Rectangle {
        id: background
        anchors.fill: parent
        opacity: useHighlight ? 1.0 : 0.0
        visible: opacity > 0
        color: Colors.listItem

        Behavior on opacity { NumberAnimation { duration: 200 } }
    }

    MouseArea {
        anchors.fill: parent
        onClicked: delegate.clicked()
    }

    Item {
        id: titlePane

        anchors {
            left: parent.left
            right: parent.right
            top: parent.top
        }
        height: childrenRect.height

        Component {
            id: contextMenu

            ContextMenu {
                model: ContextMenuModel {
                    ContextMenuEntry {
                        name: qsTr( "Rename" )
                    }
                    ContextMenuEntry {
                        name: qsTr( "Delete" )
                        symbol: Symbols.trash
                    }
                }

                onClicked: {
                    var dialog;
                    switch ( index ) {
                    case 0:
                        dialog = renameDialog.createObject( delegate );
                        dialog.show();
                        break;
                    case 1:
                        dialog = deleteDialog.createObject( delegate );
                        dialog.show();
                        break;
                    default:
                        break;
                    }
                }
            }
        }

        Component {
            id: renameDialog

            LineEditDialog {
                inputLabel: qsTr( "Please specify a new name for the todo list:" )
                text: todoList.name
                onTextAvailable: todoList.name = text
            }
        }

        Component {
            id: deleteDialog

            Overlay {
                Rectangle {
                    anchors.fill: parent
                    anchors.margins: Measures.tinySpace
                    color: Colors.window
                    border {
                        color: Colors.border
                        width: Measures.smallBorderWidth
                    }

                    Label {
                        id: deleteCaption
                        font.bold: true
                        text: qsTr( "Do you want to delete this todo list?" )
                        anchors {
                            left: parent.left
                            right: parent.right
                            top: parent.top
                            margins: Measures.tinySpace
                        }
                        wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                    }

                    Label {
                        text: qsTr( "Deleting a todo list will also delete any todos contained in it. " +
                                    "Note that this cannot be undone. Do you want to proceed?" )
                        anchors {
                            left: parent.left
                            right: parent.right
                            top: deleteCaption.bottom
                            margins: Measures.tinySpace
                        }
                        wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                    }

                    Button {
                        id: cancelDeleteButton
                        text: qsTr( "Cancel" )
                        anchors {
                            right: parent.right
                            bottom: parent.bottom
                            margins: Measures.tinySpace
                        }

                        onClicked: close()
                    }

                    Button {
                        text: qsTr( "Delete" )
                        anchors {
                            right: cancelDeleteButton.left
                            bottom: parent.bottom
                            margins: Measures.tinySpace
                        }
                        onClicked: {
                            todoList.dispose();
                            close();
                        }
                    }
                }
            }
        }

        SymbolLabel {
            id: label

            anchors {
                left: parent.left
                right: menuButton.left
                top: parent.top
                margins: Measures.tinySpace
            }
            symbol: showTodos ? Symbols.doubleUp : ""
            text: todoList.name
            placeholder: qsTr( "Untitled Todo List" )

            onClicked: delegate.clicked()
        }

        SymbolLink {
            id: menuButton

            anchors {
                right: parent.right
                top: parent.top
                margins: Measures.tinySpace
            }
            minHeight: label.height
            symbol: Symbols.verticalEllipsis

            onClicked: {
                var menu = contextMenu.createObject( this );
                menu.popup( menuButton );
            }
        }
    }

    TodoView {
        id: todosView
        clip: true
        anchors {
            left: parent.left
            right: parent.right
            top: titlePane.bottom
            bottom: parent.bottom
            margins: Measures.tinySpace
            leftMargin: Measures.smallSpace
        }
        todos: TodoModel {
            queryType: TodoModel.InvalidQuery
            todoList: delegate.showTodos ? delegate.todoList : null
        }
        visible: delegate.showTodos

        onTodoSelected: delegate.todoSelected( todo )
    }

    Rectangle {
        visible: delegate.drawDivider
        anchors {
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }

        height: Measures.smallBorderWidth
        color: Colors.listItem
    }

    Behavior on height { NumberAnimation { duration: 200 } }

}
