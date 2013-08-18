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
import "Utils.js" as Utils

Item {
    id: button
    
    signal clicked
    
    property alias label: text.text
    property int sourceIndex: 0
    property alias containsMouse: mouseArea.containsMouse
    property int childOffset: 10
    property bool enabled: true
    property alias font: text.font
    
    width: text.width + text.height * 2
    height: text.height * 2

    Rectangle {
        id: background
        width: parent.width
        height: text.height * 4
        color: colors.primary

        Behavior on color {
            ColorAnimation {
                duration: 100
            }
        }
    }
    
    Text {
        id: text
        text: "ToolButton"
        font.weight: Font.DemiBold
        font.pointSize: fonts.h1
        color: colors.fontColorFor( background.color )
        anchors.horizontalCenter: parent.horizontalCenter
        y: childOffset + 10
        wrapMode: Text.WordWrap
    }
    
    MouseArea {
        id: mouseArea
        anchors.fill: parent
        hoverEnabled: true
        cursorShape: button.state == "disabled" ? Qt.ArrowCursor : Qt.PointingHandCursor
        onClicked: if ( button.state != "disabled" ) parent.clicked()
    }
    
    states: [
        State {
            name: "hovered"
            when: containsMouse && !( parent.disabled || !enabled )
            PropertyChanges {
                target: background
                color: colors.primaryLighter1
            }
        },
        State {
            name: "disabled"
            when: parent.disabled || !enabled
            PropertyChanges {
                target: button
                opacity: 0
                childOffset: height
            }
        }
    ]
    
    transitions: [
        Transition {
            from: ""
            to: "hovered"
            reversible: true
            NumberAnimation {
                properties: "childOffset"
                duration: 100
                easing.type: Easing.InOutQuad
            }
        },
        Transition {
            //from: ""
            to: "disabled"
            reversible: true
            ParallelAnimation {
                NumberAnimation {
                    properties: "opacity"
                    duration: 200
                    easing.type: Easing.InOutQuad
                }
                NumberAnimation {
                    properties: "childOffset"
                    duration: 200
                    easing.type: Easing.InOutQuad
                }
            }
        }
    ]
    
}
