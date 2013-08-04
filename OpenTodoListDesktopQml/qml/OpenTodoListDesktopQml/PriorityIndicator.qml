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
    id: indicator

    width: 100
    height: 100

    property int priority: 0

    signal selectedPriority(int priority)

    Keys.onEscapePressed: focus = false

    Item {
        id: helper
        readonly property color noColor: "#00000000"
        property color hoveredColor: noColor
        function noColorSelected() { return hoveredColor === noColor; }
        Behavior on hoveredColor { ColorAnimation { duration: 200 } }
    }

    Image {
        anchors {
            fill: parent
        }
        source: "image://primitives/pie/percentage=100,fill=" +
                ( helper.noColorSelected() ? Utils.PriorityColors[priority] :
                                            helper.hoveredColor )
    }

    MouseArea {
        id: indicatorMouseArea
        anchors.fill: parent
        hoverEnabled: true
        onClicked: indicator.focus = !indicator.focus
    }

    Text {
        id: text
        opacity: indicator.focus ? 0 : 1
        anchors.centerIn: parent
        text: indicator.priority >= 0 ? "Priority " + indicator.priority : "No Priority"
        font.pointSize: fonts.p
        color: colors.fontColorFor( Utils.PriorityColors[indicator.priority] )
        Behavior on opacity { NumberAnimation { duration: 300 } }
    }

    Item {
        anchors {
            fill: parent
            margins: 5
        }
        Repeater {
            model: 12
            Item {
                opacity: indicator.focus ? 1 : 0
                visible: opacity !== 0
                width: indicator.width / 6
                height: indicator.height * 9 / 10
                rotation: index * 360 / 12
                anchors.horizontalCenter: parent.horizontalCenter
                Behavior on opacity { NumberAnimation { duration: 300 } }
                Image {
                    width: parent.width
                    height: parent.width
                    source: "image://primitives/pie/percentage=100,fill=" +
                            text.color
                }
                Image {
                    width: parent.width - 4
                    height: parent.width -4
                    x: 2
                    y: 2
                    source: "image://primitives/pie/percentage=100,fill=" +
                            Utils.PriorityColors[index-1]
                }
                MouseArea {
                    width: parent.width
                    height: parent.width
                    hoverEnabled: true
                    onClicked: {
                        indicator.selectedPriority( index - 1 )
                        indicator.focus = false;
                    }
                    onContainsMouseChanged: helper.hoveredColor =
                                            containsMouse ?
                                                Utils.PriorityColors[index-1] :
                                                helpers.noColor
                }
            }
        }
    }
}
