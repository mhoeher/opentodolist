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
    color: colors.input
    height: textInput.height + 10
    clip: true
    border.width: 1
    border.color: Qt.darker( colors.button )
    
    property alias text: textInput.text
    property string placeholderText: ""

    // Indicates that return or enter has been pressed.
    // Useful to implement a "default" action or
    // provide a shortcut (so user does not have to hit a button
    // next to the input).
    signal apply()

    Keys.onEnterPressed: apply();
    Keys.onReturnPressed: apply();

    MouseArea {
        anchors.fill: parent
        onClicked: { textInput.focus = true; }
    }

    Text {
        id: placeholder
        text: input.placeholderText
        opacity: ( textInput.text == "" && !textInput.focus ) ? 0.5 : 0
        anchors.verticalCenter: parent.verticalCenter
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.margins: 5
        font.pointSize: fonts.p
        Behavior on opacity { NumberAnimation { duration: 300 } }
    }
    
    TextInput {
        id: textInput
        anchors.verticalCenter: parent.verticalCenter
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.margins: 5
        text: "Simple Text Input"
        color: colors.fontColorFor( input.color )
        font.pointSize: fonts.p
        Keys.onEscapePressed: focus = false
        Keys.onBackPressed: focus = false
    }
}
