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
    id: button
    height: label.height + 10
    width: label.width + 20
    color: activePalette.button
    gradient: Gradient {
        GradientStop {
            position: 0.0
            color: Qt.lighter( backgroundColor )
        }
        GradientStop {
            position: 1.0
            color: activePalette.button
        }
    }
    radius: 4
    border.width: 1
    border.color: Qt.darker( backgroundColor )
    
    property alias label: label.text
    property alias pressed: mouseArea.pressed
    property alias containsMouse: mouseArea.containsMouse
    property color backgroundColor: activePalette.button
    
    signal clicked
    
    Text {
        id: label
        text: "Button"
        color: activePalette.buttonText
        x: 10
        y: 5
    }
    
    MouseArea {
        anchors.fill: parent
        id: mouseArea
        hoverEnabled: true
        
        onClicked: button.clicked()
    }
    
    states: [
        State {
            name: "down"
            when: pressed || containsMouse
            PropertyChanges {
                target: button
                backgroundColor: Qt.darker( activePalette.button )
            }
        }
    ]
    
    transitions: [
        Transition {
            from: ""
            to: "down"
            reversible: true
            ColorAnimation {
                duration: 200
                easing.type: Easing.InOutQuad
            }
        }
    ]
    
}