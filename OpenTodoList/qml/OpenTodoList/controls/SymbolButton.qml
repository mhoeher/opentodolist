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

Item {
    id: button
    width: childrenRect.width + layout.minimumButtonHeight / 2
    height: childrenRect.height + layout.minimumButtonHeight / 2

    property alias text: text.text
    property alias color: text.color
    property alias font: text.font

    signal clicked()

    Text {
        id: text
        x: layout.minimumButtonHeight / 4
        y: layout.minimumButtonHeight / 4
        font.pointSize: fonts.h1
        font.family: symbolFont.name
    }

    MouseArea {
        anchors.fill: text
        onClicked: button.clicked()
    }

}

