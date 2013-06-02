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

View {
    id: todoListView
    hidden: false
    
    property QtObject currentList : null
    signal todoSelected(QtObject todo)
    
    toolButtons: [
    Button {
        label: "New List"
        
        onClicked: newTodoListView.hidden = false
    },
    Button {
        label: "Quit"
        
        onClicked: Qt.quit()
    }
    ]
    
    Item {
        id: sideBar
        
        width: todoListView.currentList ? 200 : todoListView.clientWidth
        height: todoListView.clientHeight
        
        Behavior on width { SmoothedAnimation { velocity: 1200 } }
        
        ListView {
            width: parent.width
            height: childrenRect.height
            model: library.todoLists
            delegate: Rectangle {
                width: parent.width
                height: todoListLabel.paintedHeight + 10
                border.width: 1
                border.color: todoListView.currentList == object ? 'red' : 'black'
                
                Text {
                    id: todoListLabel
                    text: object.name
                }
                
                MouseArea {
                    anchors.fill: parent
                    onClicked: todoListView.currentList = object
                }
            }
        }
        
    }
    
    Item {
        id: todoListContents
        anchors.left: sideBar.right
        anchors.leftMargin: 10
        width: todoListView.clientWidth - 200 - 10
        Column {
            spacing: 5
            
            Item {
                height: childrenRect.height
                width: todoListContents.width
                SimpleTextInput {
                    id: newTodoTitle
                    anchors { left: parent.left; right: parent.right; rightMargin: addNewTodoButton.width + 10 }
                    text: ""
                }
                Button {
                    id: addNewTodoButton
                    label: "Add"
                    
                    anchors.right: parent.right
                    
                    onClicked: {
                        var todo = todoListView.currentList.addTodo();
                        todo.title = newTodoTitle.text;
                        newTodoTitle.text = "";
                    }
                }
            }
            
            ListView {
                model: todoListView.currentList ? todoListView.currentList.entries : null
                width: todoListContents.width
                height: todoListView.clientHeight
                clip: true
                delegate: TodoListEntry {
                    todo: object
                    onClicked: {
                        todoListView.todoSelected( object )
                    }
                }
            }
        }
    }
    
}