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
import net.rpdev.OpenTodoList.Theme 1.0

Overlay {
    id: dialog

    property alias focusDate: calendar.focusDate
    property alias highlightDates: calendar.highlightDates

    signal dateCleared()
    signal dateSelected( date selectedDate )

    Rectangle {
        anchors.fill: parent
        anchors.margins: Measures.tinySpace
        color: Colors.window
        border {
            width: Measures.smallBorderWidth
            color: Colors.border
        }

        Label {
            id: inputPromt

            text: qsTr( "Please select a date:" )
            anchors {
                left: parent.left
                right: parent.right
                top: parent.top
                margins: Measures.tinySpace
            }
            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
        }

        Calendar {
            id: calendar

            anchors {
                left: parent.left
                right: parent.right
                top: inputPromt.bottom
                bottom: buttonBar.top
                margins: Measures.tinySpace
            }

            onDateClicked: {
                dialog.dateSelected( selectedDate );
                dialog.close();
            }

        }

        Item {
            id: buttonBar

            height: childrenRect.height
            anchors {
                bottom: parent.bottom
                left: parent.left
                right: parent.right
                margins: Measures.tinySpace
            }

            Button {
                id: closeButton

                text: qsTr( "Close" )
                onClicked: dialog.close()
                anchors {
                    right: parent.right
                }
            }

            Button {
                id: clearButton

                onClicked: {
                    dialog.dateCleared();
                    dialog.close();
                }

                text: qsTr( "Clear Date" )
                anchors {
                    right: closeButton.left
                    margins: Measures.tinySpace
                }
            }

        }
    }
}
