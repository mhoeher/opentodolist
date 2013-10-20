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
import QtQuick.Window 2.0
import QtQuick.Layouts 1.0
import QtQuick.Controls 1.0

Window {

    id: dialog

    property alias name: nameEdit.text
    property QtObject todoList: null

    modality: Qt.ApplicationModal
    width: 400
    height: 200

    function edit( list ) {
        todoList = list;
        nameEdit.text = list.name;
        title = qsTr( "Edit \"%1\"" ).arg( todoList.name );
        show();
    }

    GridLayout {
        columns: 2
        anchors.fill: parent
        anchors.margins: 10

        Label {
            text: qsTr( "Name:" )
        }

        TextField {
            id: nameEdit
            Layout.fillWidth: true
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
                    onClicked: {
                        dialog.todoList.name = nameEdit.text;
                        dialog.close();
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
