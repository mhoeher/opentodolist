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

Item {
    id: root

    property int spacing: 4
    property QtObject model: null
    property bool autoSize: false
    property int contentHeight: controls.childrenRect.height + listView.contentHeight

    signal todoSelected(QtObject todo)

    clip: true
    width: 100
    height: 100

    Item {
        id: controls
        anchors { top: parent.top; left: parent.left; right: parent.right }
        height: childrenRect.height
        Behavior on height { SmoothedAnimation { duration: 500 } }
    }
    Item {
        id: list
        anchors { top: controls.bottom; left: parent.left; right: parent.right; bottom: parent.bottom }
        ListView {
            id: listView
            anchors.fill: parent
            model: TodoSortFilterModel {
                sourceModel: root.model
                sortMode: TodoSortFilterModel.PrioritySort
            }

            delegate: TodoListEntry {
                todo: object
                hasNext: object !== listView.model.getItem( listView.model.itemCount() - 1 ).object
                onClicked: {
                    root.todoSelected( object )
                }
            }
        }
    }
}

