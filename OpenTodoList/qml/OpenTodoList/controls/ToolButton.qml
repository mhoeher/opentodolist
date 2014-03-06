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
import "../js/Utils.js" as Utils

Item {
    id: button
    
    signal clicked
    
    property alias symbol: symbolLabel.text
    property alias label: textLabel.text
    property alias containsMouse: contents.containsMouse
    property bool enabled: true
    property bool isBackButton: false

    width: enabled ? contents.width : 0
    height: contents.height * 2
    clip: true

    Behavior on width { NumberAnimation { duration: 200 } }

    MouseArea {
        id: contents

        hoverEnabled: true
        cursorShape: button.state == "disabled" ? Qt.ArrowCursor : Qt.PointingHandCursor
        onClicked: if ( button.state != "disabled" ) parent.clicked()
        width: row.width * 2.5
        height: row.height
        anchors.verticalCenter: parent.verticalCenter

        Row {
            id: row
            spacing: symbolLabel.height / 2
            anchors.horizontalCenter: parent.horizontalCenter
            Text {
                id: symbolLabel
                font.family: symbolFont.name
                font.weight: Font.DemiBold
                font.pointSize: fonts.h1
                color: "white"
            }
            Text {
                id: textLabel
                font.weight: Font.DemiBold
                font.pointSize: fonts.h1
                color: "white"
            }
        }
    }

}
