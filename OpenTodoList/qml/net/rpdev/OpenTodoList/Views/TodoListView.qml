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
import net.rpdev.OpenTodoList.Core 1.0
import net.rpdev.OpenTodoList.Components 1.0
import net.rpdev.OpenTodoList.Theme 1.0

ListView {
    id: todoListView

    property TodoListLibrary library
    property TodoList currentTodoList
    property bool showTodosInline: false

    signal todoListSelected( TodoList todoList )

    highlightMoveDuration: 200
    currentIndex: -1

    model: todoListModel

    delegate: TodoListViewDelegate {
        todoList: display
        maxHeight: todoListView.height
        drawDivider: index < todoListModel.count - 1
        onClicked: {
            currentIndex = index;
            currentTodoList = display;
            if ( showTodos ) {
                showTodos = false;
            } else {
                todoListSelected( display );
                showTodos = Qt.binding( function() { return showTodosInline && currentIndex == index; } );
            }
        }
    }

    highlight: Item {
        width: parent.width
        Rectangle {
            anchors {
                left: parent.left
                top: parent.top
                bottom: parent.bottom
                bottomMargin: Measures.tinySpace * 2
            }
            width: Measures.tinySpace
            gradient: Gradient {
                GradientStop {
                    color: Colors.secondary1Light1
                    position: 0.0
                }
                GradientStop {
                    color: Colors.secondary1
                    position: 0.5
                }
                GradientStop {
                    color: Colors.secondary1Light1
                    position: 1.0
                }
            }
        }
    }

    TodoListModel {
        id: todoListModel
        library: todoListView.library
    }
}
