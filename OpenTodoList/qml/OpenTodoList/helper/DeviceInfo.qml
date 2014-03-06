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

    readonly property bool isAndroid: Qt.platform.os === "android"
    readonly property bool isBlackberry: Qt.platform.os === "blackberry"
    readonly property bool isIOS: Qt.platform.os === "ios"
    readonly property bool isLinux: Qt.platform.os === "linux"
    readonly property bool isOSX: Qt.platform.os === "osx"
    readonly property bool isUnix: Qt.platform.os === "unix"
    readonly property bool isWindows: Qt.platform.os === "windows"
    readonly property bool isWinCE: Qt.platform.os === "wince"


    /*
      Are we on a touch device?
      For some design decisions, this fact is used as an input for decision.
      For "guessing" this, we're checking the current operating system.
      */
    property bool isTouchDevice: isAndroid || isIOS || isBlackberry || isWinCE
}
