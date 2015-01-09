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

pragma Singleton

import QtQuick 2.4
import QtQuick.Controls 1.2

QtObject {
    property Action quit: Action {
        text: qsTr( "Quit")
        shortcut: StandardKey.Quit
        onTriggered: application.handler.terminateApplication()
    }
    property Action close: Action {
        text: qsTr( "Close" )
        shortcut: StandardKey.Close
        onTriggered: application.handler.hideWindow()
    }
}

