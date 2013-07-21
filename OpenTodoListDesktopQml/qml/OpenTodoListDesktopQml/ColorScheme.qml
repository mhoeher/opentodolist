/*
 *  OpenTodoListDesktopQml - Desktop QML frontend for OpenTodoList
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

/*
  This component encapsulates the color scheme to use.

  Todo: This potentially should use the current color scheme of the desktop
        environment if we are e.g. in KDE or Gnome.
  */
Item {
    id: colorScheme

    // Primary color definitions:
    property color primary: "#1F5900"
    property color primaryDarker1: "#306314"
    property color primaryDarker2: "#2B7C00"
    property color primaryLighter1: "#55B123"
    property color primaryLighter2: "#67B140"

    // Complementary color definitions:
    property color complementaryColor: "#5F0014"
    property color complementaryColorDarker1: "#6A1527"
    property color complementaryColorDarker2: "#84001C"
    property color complementaryColorLighter1: "#B52443"
    property color complementaryColorLighter2: "#B5415A"

    // Background color definitions
    property color window: "#FCFEFC"
    property color button: Qt.darker( colorScheme.window, 1.5 )
    property color input: Qt.lighter( colorScheme.window, 1.5 )

    // See: http://stackoverflow.com/questions/3942878/how-to-decide-font-color-in-white-or-blackdepending-on-background-color
    // This function selects a font color depending on the given background color.
    function fontColorFor( color ) {
        if ( color.r * 0.299 + color.g * 0.587 + color.b * 0.114 > 0.729 ) {
            return "black";
        } else {
            return "white";
        }
    }
}
