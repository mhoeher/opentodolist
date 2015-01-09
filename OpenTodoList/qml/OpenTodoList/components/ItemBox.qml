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

import QtQuick 2.4
import QtGraphicalEffects 1.0

import "../style" as Style

Item {
    id: box

    property alias color: background.color

    Rectangle {
        id: background
        color: Style.Colors.listItem

        anchors.fill: parent
        anchors.margins: Style.Measures.tinySpace

        border {
            width: Style.Measures.smallBorderWidth
            color: Style.Colors.border
        }
    }

    ShaderEffectSource {
        id: effectSource
        sourceItem: background
        anchors.fill: background
        hideSource: true
        visible: false
    }

    DropShadow {
        anchors.fill: effectSource
        horizontalOffset: Style.Measures.extraTinySpace
        verticalOffset: Style.Measures.extraTinySpace
        radius: Style.Measures.largeSpace
        samples: 32
        color: "#20000000"
        source: effectSource
        visible: true
        spread: 0
    }

}

