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
    id: component

    property alias symbol: symbol.text
    property alias color: symbol.color
    property alias font: symbol.font
    property alias text: symbol.text

    width: Math.max( symbol.width, symbol.height, Measures.mHeight )
    height: width

    Label {
        id: symbol

        font.family: Fonts.symbols.name
        font.pixelSize: Measures.mHeight * 1.1
        anchors.centerIn: parent
    }
}
