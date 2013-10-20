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

import "../components"

Window {
    id: dialog
    width: contentItem.childrenRect.width
    height: contentItem.childrenRect.height
    modality: Qt.ApplicationModal
    title: qsTr( "Select a date..." )

    property var date: null

    function exec( date ) {
        dialog.date = date;
        show();
    }

    function apply() {
        if ( date ) {
            dateSelected( date );
        }
    }

    signal dateSelected( date date )

    Component.onCompleted: {
        applicationViewer.onBeforeReload.connect( close );
    }

    Column {

        Calendar {
            id: calendar
            selectedDate: dialog.date

            onSelectedDateChanged: dialog.date = selectedDate
        }

        Item {
            height: childrenRect.height * 2
            width: calendar.width

            Row {
                anchors.right: parent.right
                anchors.bottom: parent.bottom
                anchors.margins: 5
                spacing: 10

                Button {
                    text: qsTr( "OK" )

                    onClicked: {
                        dialog.apply();
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
