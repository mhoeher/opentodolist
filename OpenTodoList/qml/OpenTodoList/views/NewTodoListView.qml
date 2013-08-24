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
import "../controls"

View {
    id: newTodoListView
    
    property alias name: newTodoListName.text
    property QtObject type: null
    
    toolButtons: [
        ToolButton {
            symbol: "\uf060"
            onClicked: newTodoListView.hidden = true
        },
        ToolButton {
            label: "Create"

            onClicked: {
                if ( newTodoListView.name != "" &&
                        library.createTodoList(
                            newTodoListView.name, newTodoListView.type ) ) {
                    newTodoListView.hidden = true;
                }
            }
        }
    ]
    
    Grid {
        spacing: 10
        columns: 2
        width: parent.clientWidth
        height: parent.clientHeight
        
        Text {
            text: "<b>List Name:</b>"
            color: newTodoListName.text != "" ? "black" : "red"
        }
        SimpleTextInput {
            id: newTodoListName
            width: parent.width * 0.8
            text: ""
        }
        Text {
            text: "<b>Type:</b>"
            color: newTodoListView.type ? "black" : "red"
        }
        ListView {
            model: library.plugins.backends
            clip: true
            width: parent.width * 0.8
            height: 400
            delegate: Button {
                label: object.name
                down: newTodoListView.type == object

                onClicked: newTodoListView.type = object
            }
        }
    }
}
