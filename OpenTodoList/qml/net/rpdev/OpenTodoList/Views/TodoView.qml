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
import net.rpdev.OpenTodoList.DataModel 1.0
import net.rpdev.OpenTodoList.Models 1.0
import net.rpdev.OpenTodoList.Theme 1.0
import net.rpdev.OpenTodoList.Views 1.0
import net.rpdev.OpenTodoList.Components 1.0

Item {
    id: todoView

    property TodoModel todos
    property alias currentIndex: view.currentIndex
    property alias color: background.color
    property alias backgroundVisible: background.visible
    property alias contentItem: view.contentItem
    property alias headerHeight: header.height
    property bool trashView: false
    property alias interactive: view.interactive
    property alias headerLabel: header.headerText
    property alias headerFont: header.headerFont

    signal todoSelected( Todo todo )

    onTodosChanged: {
        if ( todos ) {
            todos.sortMode = Qt.binding( function() { return ViewSettings.todoSortMode; } );
            todos.showDone = Qt.binding( function() { return trashView ? true : ViewSettings.showDoneTodos; } );
        }
    }

    Rectangle {
        id: background
        anchors.fill: parent
        color: Colors.listItem
        visible: false
    }

    TodoViewHeader {
        id: header
        anchors {
            left: parent.left
            top: parent.top
            right: parent.right
        }
        todos: todoView.todos
        trashView: todoView.trashView
    }

    AnimatedListView {
        id: view

        model: todoView.todos
        currentIndex: -1
        highlightMoveDuration: 200
        clip: true
        anchors {
            left: parent.left
            top: header.bottom
            right: parent.right
            bottom: parent.bottom
        }

        delegate: TodoViewDelegate {
            todo: display
            onClicked: {
                currentIndex = index;
                todoView.todoSelected( display );
            }
        }
    }
}
