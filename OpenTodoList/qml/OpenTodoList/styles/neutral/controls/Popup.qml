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

Rectangle {
    id: popup

    property bool showing: false

    function show( options ) {
        repeater.model = options
        showing = true
    }

    function hide( result ) {
        showing = false
        popup.result( result )
    }

    signal result( variant id )

    Keys.onEscapePressed: hide()
    Keys.onBackPressed: hide()
    focus: true

    anchors.fill: parent
    color: colors.popup
    opacity: showing ? 1 : 0
    visible: opacity != 0

    Behavior on opacity { NumberAnimation { duration: 200 } }

    MouseArea {
        anchors.fill: parent
        onClicked: popup.hide()
    }

    Column {
        anchors.centerIn: parent
        spacing: 10
        Repeater {
            id: repeater
            Button {
                label: modelData.label
                color: Qt.darker( colors.primaryLighter1, 1.2 )
                highlightColor: Qt.lighter( colors.primaryLighter1, 1.1 )
                padding: 10
                onClicked: popup.hide( modelData.id )
                width: popup.width / 2
                autoSize: true
            }
        }
    }
}
