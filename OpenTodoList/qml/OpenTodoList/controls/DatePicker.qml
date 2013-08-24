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
import "../js/Utils.js" as Utils

Item {
    id: root

    height: childrenRect.height
    width: childrenRect.width
    clip: true

    property date date: new Date()
    property bool selecting: false
    property int baseHeight: 100
    property int baseWidth: 100

    Flipable {
        id: flipable
        width: front.width
        height: front.width

        front: Item {
            height: childrenRect.height
            width: childrenRect.width
            Column {
                id: emblem
                Text {
                    font.family: symbolFont.name
                    font.pixelSize: root.baseHeight * 0.75
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: Utils.isValidDate( root.date ) ? "\uf073" : "\uf133"
                }
                Text {
                    font.pixelSize: root.baseHeight * 0.2
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: Utils.formatDate( root.date, "No Due Date" )
                }
            }
            MouseArea {
                anchors.fill: emblem
                enabled: !root.selecting
                onClicked: {
                    calendar.selectedDate = root.date
                    root.selecting = true
                }
            }
        }

        back: Item {
            height: editColumn.height
            width: editColumn.width
            Column {
                id: editColumn
                Calendar {
                    id: calendar
                }
                Row {
                    spacing: 5
                    Button {
                        label: "OK"
                        onClicked: {
                            root.date = calendar.selectedDate
                            root.selecting = false
                        }
                    }
                    Button {
                        label: "Cancel"
                        onClicked: root.selecting = false
                    }
                    Button {
                        label: "Reset"
                        onClicked: {
                            root.date = Utils.getNullDate()
                            root.selecting = false
                        }
                    }
                }
            }
        }

        transform: Rotation {
            id: rotation
            axis { x: 0; y: 1; z: 0 }
            origin.x: root.width/2
            angle: 0
        }

        states: State {
            name: "flipped"
            when: root.selecting
            PropertyChanges { target: rotation; angle: 180 }
            PropertyChanges { target: flipable; width: flipable.back.width; height: flipable.back.height }
        }

        transitions: Transition {
            reversible: true
            ParallelAnimation {
                NumberAnimation {
                    id: rotationAnimation
                    target: rotation
                    property: "angle"
                    duration: 500
                }
                NumberAnimation {
                    target: flipable
                    property: "width"
                    duration: rotationAnimation.duration
                }
                NumberAnimation {
                    target: flipable
                    property: "height"
                    duration: rotationAnimation.duration
                }
            }
        }
    }
}
