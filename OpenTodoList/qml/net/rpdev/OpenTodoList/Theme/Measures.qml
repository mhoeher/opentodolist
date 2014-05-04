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

pragma Singleton
import QtQuick 2.2

QtObject {
    id: measures

    readonly property int mWidth: hiddenText.width
    readonly property int mHeight: hiddenText.height

    readonly property int smallBorderWidth: 1
    readonly property int midBorderWidth: 2
    readonly property int largeBorderWidth: 4
    readonly property int extraLargeBorderWidth: 8


    readonly property int extraTinySpace: mWidth * 0.25
    readonly property int tinySpace: mWidth * 0.5
    readonly property int smallSpace: mWidth
    readonly property int midSpace: mWidth * 2
    readonly property int largeSpace: mWidth * 4
    readonly property int extraLargeSpace: mWidth * 8

    readonly property int minimumPageWidth: 15 * mWidth

    property Text hiddenText: Text {
        id: hiddenText
        text: qsTr("M")
    }

}
