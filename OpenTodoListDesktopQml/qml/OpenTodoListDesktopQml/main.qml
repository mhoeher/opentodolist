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

Rectangle {
    id: root
    width: 800
    height: 600
    color: activePalette.window
    
    /*
     * Not working, see https://bugreports.qt-project.org/browse/QTBUG-26317
     *    SystemPalette { 
     *        id: activePalette
     }
     */
    // Workaround:
    Item {
        id: activePalette
        property color window: "#eeeeee"
        property color button: "#dddddd"
        property color buttonText: "#000000"
    }
    
    TodoListView {
        id: todoListView
        
        onTodoSelected: todoDetailsView.todo = todo
    }
    
    View {
        id: newTodoListView
        
        property alias name: newTodoListName.text
        property QtObject type: null
        
        toolButtons: [
        Button {
            label: "Create"
            
            onClicked: {
                if ( library.createTodoList( newTodoListView.name, newTodoListView.type ) ) {
                    newTodoListView.hidden = true;
                }
            }
        },
        Button {
            label: "Cancel"
            
            onClicked: newTodoListView.hidden = true
        }
        ]
        
        Grid {
            spacing: 5
            columns: 2
            anchors.fill: parent
            
            Text {
                text: "<b>List Name:</b>"
            }
            SimpleTextInput {
                id: newTodoListName
                width: parent.width * 0.8
            }
            Text {
                text: "<b>Type:</b>"
            }
            ListView {
                model: library.plugins.backends
                clip: true
                width: parent.width * 0.8
                height: 400
                delegate: Rectangle {
                    width: parent.width
                    height: childrenRect.height
                    radius: 5
                    color: object == newTodoListView.type ? Qt.darker( activePalette.button, 1.1 ) : activePalette.button
                    Text {
                        text: object.name
                    }
                    MouseArea {
                        anchors.fill: parent
                        onClicked: newTodoListView.type = object
                    }
                }
            }
        }
    }
       
    TodoDetailsView {
        id: todoDetailsView
    }
    
    
}
             