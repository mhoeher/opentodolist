/*
 *  OpenTodoList - A todo and task manager
 *  Copyright (C) 2014  Martin Höher <martin@rpdev.net>
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

FocusScope {
    id: edit

    property alias text: input.text
    property alias placeholder: placeholderLabel.text

    width: Measures.mWidth * 20
    height: Measures.mHeight * 2

    Keys.onEscapePressed: focus = false
    Keys.onBackPressed: focus = false

    Rectangle {
        anchors.fill: parent
        border.color: Colors.border
        border.width: Measures.smallBorderWidth
        radius: Measures.extraTinySpace
        clip: true

        TextInput {
            id: input
            focus: true
            text: ""
            clip: true
            anchors {
                left: parent.left;
                top: parent.top;
                bottom: parent.bottom;
                right: clear.left;
                margins: Measures.tinySpace
            }
            verticalAlignment: TextInput.AlignVCenter
        }
        Label {
            id: placeholderLabel
            color: Colors.midText
            text: ""
            clip: true
            visible: input.text === "" && !edit.focus
            anchors { fill: input; margins: input.anchors.margins }
            verticalAlignment: Text.AlignVCenter
        }
        Symbol {
            id: clear
            symbol: deleteText
            visible: input.text !== ""
            anchors {
                right: parent.right;
                top: parent.top;
                bottom: parent.bottom;
                margins: Measures.tinySpace
            }
            verticalAlignment: Text.AlignVCenter
        }
        MouseArea {
            anchors.fill: clear
            onClicked: input.text = ""
        }
    }
}
