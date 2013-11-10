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

Flow {
    id: root

    property alias color: symbol.color
    property alias symbol: symbol.text
    property alias text: text.text
    property alias pointSize: symbol.font.pointSize

    height: childrenRect.height
    spacing: 10

    Text {
        id: symbol
        font.family: symbolFont.name
        font.pointSize: fonts.p
        color: colors.infoText
    }

    Text {
        id: text
        font.pointSize: symbol.font.pointSize
        color: symbol.color
        text: "Info"
    }
}
