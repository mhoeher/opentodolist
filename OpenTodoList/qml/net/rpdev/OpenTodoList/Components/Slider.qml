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

Item {
    id: slider

    width: 100
    height: Measures.mHeight * 2

    property int minimum: 0
    property int maximum: 100
    property int value: 50
    property color color: Colors.secondary1

    readonly property double percentage: ( value - minimum ) / ( maximum - minimum )

    Rectangle {
        id: background

        anchors {
            left: parent.left
            right: parent.right
            verticalCenter: parent.verticalCenter
        }
        height: parent.height / 4
        color: Colors.window
        border {
            color: Colors.border
            width: Measures.smallBorderWidth
        }
        radius: 5
    }

    Rectangle {
        id: activeBackground

        anchors {
            left: parent.left
            verticalCenter: parent.verticalCenter
        }
        height: background.height
        width: handle.x + handle.width / 2
        gradient: Gradient {
            GradientStop {
                position: 0.0
                color: Qt.lighter( slider.color, 1.5 )
            }
            GradientStop {
                position: 1.0
                color: slider.color
            }
        }
        border.color: background.border.color
        border.width: background.border.width
        radius: background.radius
    }

    Rectangle {
        id: handle

        height: parent.height / 2
        width: height / 2
        radius: background.radius
        anchors.verticalCenter: parent.verticalCenter
        x: ( slider.width - width ) * percentage
        color: Colors.window
        border.width: Measures.smallBorderWidth
        border.color: slider.color

        MouseArea {
            id: sliderMouse
            anchors {
                centerIn: parent
            }
            width: Math.max( parent.width, Measures.mHeight * 2 )
            height: Math.max( parent.height, Measures.mHeight * 2 )
            drag.target: parent
            drag.axis: Drag.XAxis
            drag.minimumX: 0
            drag.maximumX: slider.width - parent.width

            onPositionChanged: {
                slider.value = ( slider.maximum - slider.minimum ) * handle.x / drag.maximumX + slider.minimum
            }
        }
    }

    Behavior on color { ColorAnimation { duration: 200 } }
}
