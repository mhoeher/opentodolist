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

    /*
      Shall we use compact layout? This is an approximation to decide on
      whether to use wide or compact layout. In case of compact layout,
      views shall display their content in a way that makes it easy to use
      on devices with rather small screens.
      */
    property bool useCompactLayout: root.width < helper.width * 6

    /*
      The minimum button height to use. This is an approximate minimal height
      that each button or other click/tapable component should get to be easily
      usable (esp. on touch devices).
      */
    property int minimumButtonHeight: helper.height * 2

    /*
      Are we on a touch device?
      For some design decisions, this fact is used as an input for decision.
      For "guessing" this, we're checking the current operating system.
      Note: double negation is to convert undefined values to false.
      */
    property bool isTouchDevice: !!os.ANDROID

    /**
      Disable hover effects?
      On some devices (esp. touch devices) hover effects work out quite badly.
      This property indicates whether hover effects should be turned off.
      */
    property bool disableHover: isTouchDevice

    Text {
        id: helper
        visible: false
        font.pointSize: 22
        text: "Helper"
    }
}
