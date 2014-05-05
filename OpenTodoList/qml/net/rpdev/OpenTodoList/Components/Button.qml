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
import net.rpdev.OpenTodoList.Components 1.0

Rectangle {
    id: button

    property alias text: label.text
    property alias font: label.font
    property alias fontColor: label.color
    property bool compact: false
    property bool checked: false
    property bool mouseDown: false
    readonly property bool active: checked || mouseDown

    signal clicked()

    gradient: Gradient {
        GradientStop {
            color: button.active ? Colors.activeButton : Colors.button
            position: 0.0

            Behavior on color { ColorAnimation { duration: 100 } }
        }
        GradientStop {
            color: Qt.darker( Colors.button, 1.2 )
            position: 1.0
        }
    }
    width: compact ? ( Math.max( label.width, label.height ) * 2 ) : ( label.width + 2 * Measures.midSpace )
    height: label.height * 2
    border {
        color: button.active ? Colors.activeBorder : Colors.border
        width: Measures.midBorderWidth

        Behavior on color { ColorAnimation { duration: 100 } }
    }
    radius: Measures.extraTinySpace

    Label {
        id: label
        text: qsTr( "Button" )
        anchors.centerIn: parent
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        onClicked: button.clicked()
        onPressed: button.mouseDown = true
        onReleased: button.mouseDown = false
        onCanceled: button.mouseDown = false
    }
}
