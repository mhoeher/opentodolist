/*
 *  OpenTodoList - A todo and task manager
 *  Copyright (C) 2014 - 2015 Martin Höher <martin@rpdev.net>
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

pragma Singleton

import QtQuick 2.2

QtObject {
    id: colors

    readonly property color window: "#eee"
    readonly property color button: "#ddd"
    readonly property color activeButton: "#b08ea9"

    readonly property color text: "black"
    readonly property color midText: "#aaa"
    readonly property color lightText: "white"
    readonly property color border: Qt.darker( button, 1.3 )
    readonly property color activeBorder: secondary1
    readonly property color listItem: Qt.darker( window, 1.05 )
    readonly property color listItemActive: Qt.darker( window, 1.5 )

    readonly property color primary: "#287500"
    readonly property color primaryLight1: "#5fba2f"
    readonly property color primaryLight2: "#77ba54"
    readonly property color primaryDark1: "#2d5816"
    readonly property color primaryDark2: "#1b4c00"

    readonly property color secondary1: "#60004e"
    readonly property color secondary1Light1: "#b02c97"
    readonly property color secondary1Light2: "#b04f9d"
    readonly property color secondary1Dark1: "#48123e"
    readonly property color secondary1Dark2: "#3f0033"

    readonly property color secondary2: "#872f00"
    readonly property color secondary2Light1: "#c36431"
    readonly property color secondary2Light2: "#c37d58"
    readonly property color secondary2Dark1: "#653419"
    readonly property color secondary2Dark2: "#571f00"

    function colorForPriority( priority, defaultColor ) {
        if ( priority >= 0 && priority <= 10 ) {
            var blendColor = Qt.rgba(
                        secondary2.r,
                        secondary2.g,
                        secondary2.b,
                        priority / 10.0 );
            return Qt.tint( primary, blendColor );
        } else {
            return defaultColor || "black";
        }
    }
}
