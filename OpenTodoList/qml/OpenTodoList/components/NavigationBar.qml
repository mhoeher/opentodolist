/*
 *  OpenTodoList - A todo and task manager
 *  Copyright (C) 2015  Martin Höher <martin@rpdev.net>
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
import "../style" as Style

Rectangle {
    id: navigationBar

    property string itemIdRole: "name"
    property string itemTitleRole: "name"
    property string itemGroupRole: "group"
    property alias items: navEntriesView.model

    signal itemSelected(string itemId)

    width: 100
    height: 62

    Rectangle {
        id: background
        width: parent.height
        height: parent.width
        x: (parent.width - width) / 2
        y: -(parent.height - height) / 2
        transform: Rotation {
            angle: 90
            origin.x: width / 2
            origin.y: height / 2
        }
        border.color: Style.Colors.border
        border.width: Style.Measures.smallBorderWidth

        gradient: Gradient {
            GradientStop {
                position: 0.0
                color: Style.Colors.darkWindowBack
            }
            GradientStop {
                position: 1.0
                color: Style.Colors.darkWindowBackAlt
            }
        }
    }

    Style.H1 {
        id: title
        text: qsTr("Navigation")
        anchors {
            left: parent.left
            right: parent.right
            top: parent.top
            margins: Style.Measures.smallSpace
        }
        wrapMode: Text.WrapAtWordBoundaryOrAnywhere
        color: Style.Colors.lightText
    }

    ListModel {
        id: mockModel
        ListElement {
            name: "Todo Lists"
            group: "Todos"
        }
        ListElement {
            name: "Search"
            group: "Todos"
        }
        ListElement {
            name: "Scheduled Todos"
            group: "Todos"
        }
        ListElement {
            name: "Quit"
            group: "Application"
        }
    }

    Component {
        id: navEntriesDelegate
        Item {
            width: parent.width
            height: Math.max( Style.Measures.optButtonHeight,
                            itemText.height )

            Style.P {
                id: itemText
                text: model[navigationBar.itemTitleRole]
                color: Style.Colors.lightText
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                width: parent.width
                anchors {
                    left: parent.left
                    leftMargin: Style.Measures.tinySpace
                    right: parent.right
                }
            }

            MouseArea {
                anchors.fill: parent
                onClicked: navigationBar.itemSelected(model[navigationBar.itemIdRole])
            }
        }
    }

    Component {
        id: navSectionDelegate
        Item {
            width: parent.width
            height: Math.max( Style.Measures.optButtonHeight,
                             0)

            Style.H6 {
                width: parent.width
                text: section
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                color: Qt.darker(Style.Colors.lightText)
            }
        }
    }

    ListView {
        id: navEntriesView
        clip: true
        model: mockModel
        delegate: navEntriesDelegate
        anchors {
            left: parent.left
            right: parent.right
            top: title.bottom
            bottom: parent.bottom
            margins: Style.Measures.midSpace
        }
        section {
            property: navigationBar.itemGroupRole
            criteria: ViewSection.FullString
            delegate: navSectionDelegate
        }
    }

}

