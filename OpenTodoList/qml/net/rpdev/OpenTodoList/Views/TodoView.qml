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
import net.rpdev.OpenTodoList.Theme 1.0
import net.rpdev.OpenTodoList.Views 1.0

Item {
    id: todoView

    property TodoModel todos
    property alias currentIndex: view.currentIndex
    property alias color: background.color
    property alias backgroundVisible: background.visible
    property alias contentItem: view.contentItem

    signal todoSelected( Todo todo )

    Rectangle {
        id: background
        anchors.fill: parent
        color: Colors.listItem
        visible: false
    }

    AnimatedListView {
        id: view

        model: todoView.todos
        currentIndex: -1
        highlightMoveDuration: 200
        anchors.fill: parent

        delegate: TodoViewDelegate {
            todo: display
            onClicked: {
                currentIndex = index;
                todoView.todoSelected( display )
            }
        }
    }
}
