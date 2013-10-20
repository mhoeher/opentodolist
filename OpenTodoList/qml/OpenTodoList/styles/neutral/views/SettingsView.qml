/*
 *  OpenTodoList - A todo and task manager
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
import "../controls"

View {
    id: settingsView

    property string currentStyle: ""

    Component.onCompleted: {
        currentStyle = applicationViewer.currentStyle;
        if ( currentStyle === "" ) {
            currentStyle = applicationViewer.platformDefaultStyle;
        }
    }

    toolButtons: [
        ToolButton {
            label: "Back"

            onClicked: settingsView.hidden = true
        },
        ToolButton {
            label: "Apply"
            onClicked: {
                applicationViewer.currentStyle = currentStyle
            }
        }
    ]

    ListView {
        anchors.fill: parent
        anchors.margins: 10
        model: applicationViewer.styleInfo

        delegate: Item {
            width: parent.width
            height: childrenRect.height + 10
            Rectangle {
                radius: 10
                color: modelData.id === settingsView.currentStyle ? colors.window : "white"
                border.color: "black"
                border.width: 2
                width: parent.width - 10
                height: childrenRect.height
                x: 5
                y: 5

                Column {
                    width: parent.width
                    LinkLabel {
                        label: modelData.name
                        width: parent.width
                        onClicked: {
                            settingsView.currentStyle = modelData.id
                        }
                    }
                    Text {
                        font.pointSize: fonts.p
                        text: modelData.description
                        width: parent.width
                        wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                    }
                }
            }
        }
    }

}
