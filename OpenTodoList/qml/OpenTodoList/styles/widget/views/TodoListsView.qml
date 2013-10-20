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

import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Window 2.0

GroupBox {
    id: view

    title: qsTr( "Todo Lists" )

    property QtObject todoList: null

    ExclusiveGroup {
        id: todoListsGroup
    }

    ScrollView {
        anchors.fill: parent

        Column {
            width: parent.parent.width

            Repeater {
                model: library.todoLists
                delegate: Item {
                    width: parent.width
                    height: childrenRect.height
                    RowLayout {
                        width: parent.width

                        RadioButton {
                            id: radioButton
                            exclusiveGroup: todoListsGroup
                            text: object.name
                            Layout.fillWidth: true

                            onCheckedChanged: {
                                if ( checked ) {
                                    view.todoList = object;
                                }
                            }
                        }
                        Item {
                            width: childrenRect.width
                            height: childrenRect.height
                            visible: radioButton.checked
                            Text {
                                id: editLabel
                                text: "Edit"
                            }
                            MouseArea {
                                anchors.fill: editLabel
                                onClicked: {
                                    var component = Qt.createComponent( "../dialogs/TodoListEditDialog.qml" );
                                    var editDialog = component.createObject( window );
                                    editDialog.edit( object );
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
