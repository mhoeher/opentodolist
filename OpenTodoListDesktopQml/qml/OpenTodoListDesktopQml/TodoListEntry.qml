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
import "Utils.js" as Utils;

Rectangle {
    id: entry
    color: Utils.tintPriority( activePalette.button, todo.priority )
    width: parent.width - 10
    x: parent.x + 5
    height: childrenRect.height
    
    property QtObject todo: null
    property alias containsMouse: mouseArea.containsMouse
    
    signal clicked
    
    MouseArea {
        id: mouseArea
        anchors.fill: parent
        hoverEnabled: true
        
        onClicked: entry.clicked()
    }
    
    Row {
        spacing: 4

        Rectangle {
            id: checkMark
            border.width: 1
            border.color: 'black'
            color: 'white'
            width: 20
            height: 20
            
            Rectangle {
                id: checkMarkInner
                width: 10
                height: 10
                x: 5
                y: 5
                color: Qt.darker( activePalette.button )
                opacity: 0
            }
            MouseArea {
                anchors.fill: parent
                onClicked: { 
                    entry.todo.progress = ( entry.todo.progress === 100 ? 0 : 100 );
                }
            }
            
            states: [ 
                State {
                    name: "checked"
                    when: entry.todo.progress === 100
                    PropertyChanges {
                        target: checkMarkInner
                        opacity: 1
                    }
                }
            ]
            
            transitions: [
                Transition {
                    from: ""
                    to: "checked"
                    reversible: true
                    NumberAnimation {
                        duration: 300
                        easing.type: Easing.Linear
                        properties: "opacity"
                    }
                }
            ]
        }
        
        Text {
            id: label
            text: entry.todo.title
            font.pointSize: 12
        }
    }


    IconButton {
        anchors.right: parent.right
        source: "action_delete.svg"
        imageWidth: 32
        imageHeight: 32

        onClicked: entry.todo.deleted = true;
    }
    
    states: [
        State {
            name: "hovered"
            when: containsMouse
            PropertyChanges {
                target: entry
                color: Qt.lighter( activePalette.button )
            }
        }
    ]
    
    transitions: [
        Transition {
            from: ""
            to: "hovered"
            reversible: true
            ColorAnimation {
                duration: 200
            }
        }
    ]
    
}
