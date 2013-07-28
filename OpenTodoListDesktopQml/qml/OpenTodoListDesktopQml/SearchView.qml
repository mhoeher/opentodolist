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
import net.rpdev.OpenTodoList 1.0

View {
    id: searchView

    property TodoSortFilterModel model : TodoSortFilterModel {
        sourceModel: library.todos
        filterMode: TodoSortFilterModel.HideDeleted
    }

    signal todoSelected(QtObject todo)

    toolButtons: [
        ToolButton {
            label: "Back"

            onClicked: searchView.hidden = true
        }

    ]

    Item {
        width: searchView.clientWidth
        height: searchView.clientHeight

        SimpleTextInput {
            id: search
            text: ""
            width: parent.width

            onTextChanged: searchView.model.searchString = text
        }

        TodoView {
            anchors.top: search.bottom
            anchors.bottom: parent.bottom
            width: parent.width
            model: search.text == "" ? null : searchView.model
            onTodoSelected: todoDetailsView.todo = todo
        }
    }
}
