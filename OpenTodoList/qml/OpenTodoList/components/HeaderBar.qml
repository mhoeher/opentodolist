/*
 *  OpenTodoList - A todo and task manager
 *  Copyright (C) 2014 - 2015  Martin Höher <martin@rpdev.net>
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

import QtQuick 2.2
import "../style" as Style

Rectangle {
    width: 100
    height: 62
    color: Style.Colors.primary

    property alias title: title.text

    signal navigationButtonClicked()
    signal menuButtonClicked()
    signal titleClicked()

    Style.H1 {
        id: navigationButton
        font.family: Style.Fonts.symbols.name
        text: Style.Symbols.bars
        color: Style.Colors.lightText
        anchors {
            left: parent.left
            leftMargin: Style.Measures.optButtonHeight / 2
            verticalCenter: parent.verticalCenter
        }
    }

    MouseArea {
        anchors {
            centerIn: navigationButton
        }
        width: Math.max( Style.Measures.optButtonHeight,
                        navigationButton.width )
        height: Math.max( Style.Measures.optButtonHeight,
                         navigationButton.height )
        onClicked: headerBar.navigationButtonClicked()
    }

    Style.H1 {
        id: title
        text: qsTr( "Title" )
        color: Style.Colors.lightText
        anchors {
            left: navigationButton.right
            right: menuButton.left
            verticalCenter: parent.verticalCenter
            margins: Style.Measures.midSpace
        }
        elide: Text.ElideRight
    }

    MouseArea {
        anchors {
            left: title.left
            right: title.right
            verticalCenter: title.verticalCenter
        }
        height: Math.max( title.height,
                         Style.Measures.optButtonHeight )
        onClicked: headerBar.titleClicked()
    }

    Style.H1 {
        id: menuButton
        font.family: Style.Fonts.symbols.name
        text: Style.Symbols.verticalEllipsis
        color: Style.Colors.lightText
        anchors {
            right: parent.right
            rightMargin: Style.Measures.optButtonHeight / 2
            verticalCenter: parent.verticalCenter
        }
    }

    MouseArea {
        anchors {
            centerIn: menuButton
        }
        width: Math.max( Style.Measures.optButtonHeight,
                        menuButton.width )
        height: Math.max( Style.Measures.optButtonHeight,
                         menuButton.height )
        onClicked: headerBar.menuButtonClicked()
    }
}

