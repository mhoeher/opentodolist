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
import QtQuick.Layouts 1.0
import QtQuick.Window 2.0
import QtQuick.Controls 1.0

Window {
    id: dialog
    width: 400
    height: 300
    modality: Qt.ApplicationModal
    title: qsTr( "Create new Todo List" )

    property QtObject selectedBackend: null

    function exec() {
        show();
    }

    Component.onCompleted: {
        applicationViewer.onBeforeReload.connect( close );
    }

    ExclusiveGroup {
        id: backendGroup
    }

    GridLayout {
        x: 0
        y: 0
        anchors.margins: 10
        anchors.fill: parent
        columns: 2

        Label {
            text: qsTr( "Todo List Name:" )
        }

        TextField {
            id: nameEdit
            Layout.fillWidth: true
            placeholderText: qsTr( "Enter a name for the list" )
        }

        Label {
            text: qsTr( "Type:" )
        }

        Column {
            width: childrenRect.width
            Repeater {
                model: library.plugins.backends
                delegate:
                    RadioButton {
                    text: object.name
                    exclusiveGroup: backendGroup
                    onCheckedChanged: {
                        if ( checked ) {
                            description.text = object.description;
                            dialog.selectedBackend = object;
                        }
                    }
                }
            }
        }

        Item { width: 1}

        Text {
            id: description
            Layout.fillWidth: true
            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
        }

        Item {
            Layout.columnSpan: 2
            Layout.fillHeight: true
            Layout.fillWidth: true

            Row {
                anchors.right: parent.right
                anchors.bottom: parent.bottom
                spacing: 10

                Button {
                    text: qsTr( "OK" )
                    enabled: nameEdit.text !== "" &&
                             dialog.selectedBackend

                    onClicked: {
                        library.createTodoList(
                                    nameEdit.text, dialog.selectedBackend );
                        close();
                    }
                }

                Button {
                    text: qsTr( "Cancel" )

                    onClicked: {
                        dialog.close();
                    }
                }
            }
        }
    }
}
