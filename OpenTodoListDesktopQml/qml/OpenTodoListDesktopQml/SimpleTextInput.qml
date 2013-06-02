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

Rectangle {
    id: input
    color: Qt.lighter( activePalette.button )
    radius: 2
    height: textInput.height + 10
    clip: true
    border.width: 1
    border.color: Qt.darker( activePalette.button )
    
    property alias text: textInput.text

    MouseArea {
        anchors.fill: parent
        onClicked: { parent.textInput.focus = true; }
    }
    
    TextInput {
        id: textInput
        anchors.verticalCenter: parent.verticalCenter
        anchors.left: parent.left
        anchors.right: parent.right
        text: "Simple Text Input"
        color: activePalette.buttonText
    }
}
