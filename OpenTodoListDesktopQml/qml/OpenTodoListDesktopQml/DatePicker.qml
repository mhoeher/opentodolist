/*
 *  OpenTodoListDesktopQml - Desktop QML frontend for OpenTodoList
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

Item {
    id: root

    height: childrenRect.height
    width: childrenRect.width

    property date date: new Date()

    function applyDate() {
        date = new Date( yearEdit.text, monthEdit.text, dateEdit.text )
    }

    Row {
        width: childrenRect.width
        spacing: 5

        SimpleTextInput {
            id: dateEdit
            text: isNaN( date.getDate() ) ? "" : date.getDate()
            placeholderText: qsTr( "DD" )
            width: 100

            onApply: applyDate()
        }
        Text {
            text: "-"
        }
        SimpleTextInput {
            id: monthEdit
            text: isNaN( date.getMonth() ) ? "" : date.getMonth()
            placeholderText: qsTr( "MM" )
            width: 100

            onApply: applyDate()
        }
        Text {
            text: "-"
        }
        SimpleTextInput {
            id: yearEdit
            text: isNaN( date.getFullYear() ) ? "" : date.getFullYear()
            placeholderText: qsTr( "YYYY" )
            width: 100

            onApply: applyDate()
        }
        IconButton {
            source: "action_delete.png"
            imageWidth: 16
            imageHeight: 16
            anchors.verticalCenter: parent.verticalCenter
            visible: !(isNaN( date.getFullYear() )
                       || isNaN( date.getMonth() )
                       || isNaN( date.getDate() ) )

            onClicked: {
                date = new Date( NaN, NaN, NaN )
            }
        }
    }
}
