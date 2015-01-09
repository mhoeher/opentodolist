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

import QtQuick 2.0
import "../style" as Style

Item {
    id: symbol

    property alias font: label.font
    property alias symbol: label.text
    property alias color: label.color
    property alias enabled: mouseArea.enabled

    signal clicked()

    width: Math.max(label.width,
                    Style.Measures.optButtonHeight)
    height: Math.max(label.height,
                     Style.Measures.optButtonHeight)

    Style.P {
        id: label
        anchors.centerIn: parent
        font.family: Style.Fonts.symbols.name
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        onClicked: symbol.clicked()
    }
}
