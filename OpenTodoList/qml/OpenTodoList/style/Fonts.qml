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
import "."

QtObject {
    id: fonts

    property FontLoader symbols: FontLoader {
        id: symbolsFont
        source: "fontawesome-webfont.ttf"
    }

    readonly property double fontStep: 0.5

    readonly property int p: Measures.fontPointSize
    readonly property double h1: h2 + fontStep
    readonly property double h2: h3 + fontStep
    readonly property double h3: h4 + fontStep
    readonly property double h4: h5 + fontStep
    readonly property double h5: h6 + fontStep
    readonly property double h6: p + fontStep

}
