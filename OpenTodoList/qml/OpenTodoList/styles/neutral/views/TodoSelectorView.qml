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

import "../controls/"

View {
    id: todoSelectorView

    signal finished( QtObject object )

    function showTodoList( list ) {
        selector.browseTodoList( list );
    }

    function showTodo( todo ) {
        selector.browseTodo( todo );
    }

    onFinished: hidden = true;

    toolButtons: [
        ToolButton {
            id: backButon
            isBackButton: true
            symbol: "\uf060"

            onClicked: finished( null )
        },
        ToolButton {
            id: selectButton
            symbol: "\uf061"
            enabled: selector.selectedObject

            onClicked: {
                if ( selector.selectedObject ) {
                    finished( selector.selectedObject )
                }
            }
        }
    ]

    LinkLabel {
        id: title
        label: qsTr( "Move to..." );
        anchors {
            left: parent.left
            right: parent.right
        }
    }

    TodoSelector {
        id: selector
        anchors {
            top: title.bottom
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }
    }
}
