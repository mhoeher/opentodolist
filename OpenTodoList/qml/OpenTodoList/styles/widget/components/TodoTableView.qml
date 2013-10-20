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
import QtQuick.Controls 1.0
import QtQuick.Controls.Styles 1.0

import net.rpdev.OpenTodoList 1.0

import "../../../js/Utils.js" as Utils

TableView {
    id: table

    property QtObject currentTodo: null

    function reset() {
        currentRow = -1;
    }

    anchors.fill: parent
    alternatingRowColors: true

    onCurrentRowChanged: {
        if ( currentRow >= 0 ) {
            currentTodo = model.getItem( currentRow ).object;
        } else {
            currentTodo = null;
        }
    }


    TableViewColumn {
        title: "Title"
        role: "object"
        delegate: CheckBox {
            id: titleDelegate
            property QtObject todo: styleData.value
            text: styleData.value.title
            //checked: styleData.value.progress === 100 ? true : false
            Component.onCompleted: checked = styleData.value.progress === 100
        }
    }

    TableViewColumn {
        title: "Priority"
        role: "object"
        delegate: Label {
            text: styleData.value.priority >= 0 ?
                      styleData.value.priority : ""
            elide: styleData.elideMode
        }
    }

    TableViewColumn {
        title: "Progress"
        role: "object"
        delegate: Label {
            text: styleData.value.progress + "%"
            elide: styleData.elideMode
        }
    }

    TableViewColumn {
        title: "Due Date"
        role: "object"
        delegate: Label {
            text: Utils.formatDate( styleData.value.dueDate, "" )
            elide: styleData.elideMode
        }
    }
}
