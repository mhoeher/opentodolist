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
import QtQuick.Layouts 1.0
import QtQuick.Window 2.0
import QtQuick.Controls 1.0

Window {
    id: dialog

    property string selectedStyle: ""

    function exec() {
        show();
    }

    width: 400
    height: 300
    modality: Qt.ApplicationModal
    title: qsTr( "Select Application Style" )

    Component.onCompleted: {
        applicationViewer.onBeforeReload.connect( close );
        selectedStyle = applicationViewer.currentStyle;
    }

    ExclusiveGroup {
        id: styleGroup
    }

    ScrollView {
        id: scrollView
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.bottom: buttonBar.top

        Column {
            width: scrollView.width
            height: childrenRect.height

            Repeater {
                model: applicationViewer.styleInfo

                delegate: Item {
                    width: parent.width
                    height: childrenRect.height

                    Column {
                        width: parent.width
                        height: childrenRect.height

                        RadioButton {
                            id: radioButton
                            text: modelData.name
                            checked: dialog.selectedStyle === modelData.id
                            exclusiveGroup: styleGroup
                            onClicked: {
                                dialog.selectedStyle = modelData.id;
                            }

                        }
                        Label {
                            text: modelData.description
                            width: parent.width
                            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                        }
                        Item {
                            width: parent.width
                            height: 10
                        }
                    }
                }
            }
        }
    }

    Item {
        id: buttonBar
        anchors { left: parent.left; right: parent.right; bottom: parent.bottom }
        height: childrenRect.height

        Row {
            spacing: 10
            anchors.right: parent.right
            anchors.rightMargin: 10
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 10

            Button {
                text: qsTr( "OK" )
                onClicked: applicationViewer.currentStyle = dialog.selectedStyle
            }

            Button {
                text: qsTr( "Cancel" )
                onClicked: dialog.close()
            }
        }
    }

}
