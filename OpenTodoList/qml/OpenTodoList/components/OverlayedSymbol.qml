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

Item {
    id: symbol

    property alias font: backLabel.font
    property alias backSymbol: backLabel.text
    property alias frontSymbol: frontLabel.text
    property alias backColor: backLabel.color
    property alias frontColor: frontLabel.color
    property alias enabled: mouseArea.enabled
    property alias drag: mouseArea.drag
    property bool checked: false

    signal clicked()
    signal released()
    signal canceled()

    width: Math.max(backLabel.width,
                    frontLabel.width,
                    Style.Measures.optButtonHeight)
    height: Math.max(backLabel.height,
                     frontLabel.height,
                     Style.Measures.optButtonHeight)

    Style.H3 {
        id: backLabel
        anchors.centerIn: parent
        font.family: Style.Fonts.symbols.name
        font.bold: false
    }

    Style.H3 {
        id: frontLabel
        anchors.centerIn: parent
        font: backLabel.font
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        onClicked: symbol.clicked()
        onReleased: symbol.released()
        onCanceled: symbol.canceled()
    }

    states: State {
        when: !visible
        PropertyChanges {
            target: symbol
            width: 0
            height: 0
        }
    }
}
