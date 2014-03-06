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

FocusScope {
    id: component

    property bool showing: false

    function show( options ) {
        component.focus = true;
        repeater.model = options
        showing = true
    }

    function hide( result ) {
        component.focus = false;
        showing = false
        component.result( result )
        component.close()
    }

    signal result( variant id )
    signal close()

    anchors.fill: parent

    Rectangle {
        id: popup

        Keys.onReleased: {
            switch ( event.key ) {
            case Qt.Key_Escape:
            case Qt.Key_Back:
                component.hide();
                event.accepted = true;
            }
        }

        focus: true

        anchors.fill: parent
        color: colors.popup
        opacity: component.showing ? 1 : 0
        visible: opacity != 0

        Behavior on opacity { NumberAnimation { duration: 200 } }

        MouseArea {
            anchors.fill: parent
            onClicked: component.hide()
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
                    onClicked: component.hide( modelData.id )
                    width: popup.width / 2
                    autoSize: true
                }
            }
        }
    }
}
