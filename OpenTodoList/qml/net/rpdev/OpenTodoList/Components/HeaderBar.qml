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

Rectangle {
    id: headerBar

    width: 100
    height: Measures.mHeight * 2
    color: Colors.primary
    property list<HeaderBarItem> items
    property alias currentIndex: view.currentIndex
    property bool showActionButton: false
    property string actionButtonTitle

    signal actionButtonClicked()

    Item {
        id: actionButton

        visible: headerBar.showActionButton
        width: visible ? ( buttonRow.width + Measures.tinySpace * 2 ) : 0
        anchors {
            left: parent.left
            top: parent.top
            bottom: parent.bottom
        }

        Row {
            id: buttonRow
            spacing: Measures.tinySpace
            anchors {
                left: parent.left
                leftMargin: Measures.tinySpace
                verticalCenter: parent.verticalCenter
            }

            Symbol {
                symbol: Symbols.doubleLeft
                anchors.verticalCenter: parent.verticalCenter
                color: Colors.lightText
            }

            Label {
                id: actionLabel
                anchors.verticalCenter: parent.verticalCenter
                text: headerBar.actionButtonTitle
                color: Colors.lightText
            }
        }
        MouseArea {
            anchors.fill: parent
            onClicked: headerBar.actionButtonClicked()
        }
    }

    ListView {
        id: view
        model: headerBar.items
        orientation: Qt.Horizontal
        clip: true
        anchors {
            left: actionButton.right
            top: parent.top
            bottom: parent.bottom
            right: parent.right
        }

        highlightFollowsCurrentItem: true

        delegate: Item {
            width: label.width + 2 * Measures.smallSpace
            anchors { top: parent.top; bottom: parent.bottom }
            Label {
                id: label
                text: itemText
                anchors.centerIn: parent
                color: Colors.lightText
            }
            MouseArea {
                anchors.fill: parent
                onClicked: view.currentIndex = index
            }
        }

        highlight: Item {
            Rectangle {
                anchors { left: parent.left; right: parent.right; bottom: parent.bottom }
                height: Measures.largeBorderWidth
                color: Colors.secondary1
            }
        }
    }
}
