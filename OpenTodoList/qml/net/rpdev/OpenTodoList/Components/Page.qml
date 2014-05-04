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

Item {
    id: page

    property string name: qsTr( "Page" )
    property PageStack pageStack: null
    default property alias children: inner.children

    function close() {
        state = "closing";
    }

    anchors {
        left: parent.left
        right: parent.right
    }
    y: page.height
    height: parent.height
    state: "available"

    Rectangle {
        id: inner
        anchors.fill: parent
        color: Colors.window
    }

    states: [
        State {
            name: "available"
            PropertyChanges {
                target: page
                y: page.height
            }
        },
        State {
            name: "visible"
            PropertyChanges {
                target: page
                y: 0
            }
        },
        State {
            name: "closing"
            PropertyChanges {
                target: page
                y: height
            }
        },
        State {
            name: "default"
            PropertyChanges {
                target: page
                y: 0
            }
        }

    ]

    transitions: [
        Transition {
            from: "available"
            to: "visible"
            SmoothedAnimation {
                duration: 500
                properties: "y"
            }
        },
        Transition {
            from: "visible"
            to: "closing"
            SequentialAnimation {
                SmoothedAnimation {
                    target: page
                    properties: "y"
                    duration: 500
                }
                ScriptAction {
                    script: {
                        page.destroy();
                    }
                }
            }
        },
        Transition {
            from: "default"
            to: "closing"
            ScriptAction {
                script: parent.close()
            }
        }
    ]
}
