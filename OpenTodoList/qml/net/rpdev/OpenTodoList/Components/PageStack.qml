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

import QtQuick 2.0
import net.rpdev.OpenTodoList.Components 1.0

FocusScope {

    id: pageStack

    signal lastPageClosing()

    function showPage( page ) {
        // Current behavior: Ensure least recently added page
        // is visible. Could be changed if required in the future
        initChildren();
    }

    function initChildren() {
        for ( var i = 0; i < children.length; ++i ) {
            var child = children[i];
            child.pageStack = pageStack;
            if ( i === 0 ) {
                child.state = "default";
            } else {
                child.state = "visible";
            }
            if ( i === children.length - 1 ) {
                child.focus = true;
            }
        }
    }

    function back() {
        if ( children.length > 1 ) {
            children[ children.length - 1 ].close();
        } else if ( children.length === 1 ) {
            lastPageClosing();
        }

    }

    function close() {
        lastPageClosing();
    }

    Keys.onEscapePressed: back()
    Keys.onBackPressed: back();

    Component.onCompleted: initChildren()
}
