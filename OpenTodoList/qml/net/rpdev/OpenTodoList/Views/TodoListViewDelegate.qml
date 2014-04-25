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

Item {
    id: delegate

    property TodoList todoList: null
    property bool showTodos: false
    property int maxHeight: 0
    property bool drawDivider: false

    signal clicked()

    anchors { left: parent.left; right: parent.right }
    height: childrenRect.height + Measures.tinySpace * 2
    clip: showTodos

    onShowTodosChanged: {
        if ( showTodos ) {
            var model = Qt.createQmlObject( "import net.rpdev.OpenTodoList.Core 1.0; TodoModel {}", this );
            model.library = todoList.library;
            model.todoList = todoList;
            model.queryType = TodoModel.QueryTopLevelTodosInTodoList;
            model.update();
            todosView.model = model;
        }
    }

    Item {
        id: background
        anchors {
            left: parent.left
            right: parent.right
            top: parent.top
            margins: Measures.tinySpace
        }
        height: showTodos ? parent.maxHeight : label.height + Measures.tinySpace * ( delegate.drawDivider ? 3 : 2 )

        Label {
            id: label
            text: todoList.name
            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
            anchors {
                margins: Measures.tinySpace
                top: parent.top
                left: parent.left
                right: parent.right
            }
        }

        Label {
            anchors.fill: label
            visible: todoList.name === ""
            color: Colors.midText
            text: qsTr( "Untitled Todo List" )
            font.italic: true
            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
        }

        MouseArea {
            anchors.fill: parent
            onClicked: delegate.clicked()
        }

        TodoView {
            id: todosView
            clip: true
            anchors {
                left: parent.left
                right: parent.right
                top: label.bottom
                bottom: parent.bottom
                margins: Measures.tinySpace
                leftMargin: Measures.smallSpace
            }
        }

        Rectangle {
            visible: delegate.drawDivider
            anchors {
                left: parent.left
                right: parent.right
                bottom: parent.bottom
            }

            height: Measures.smallBorderWidth
            color: Colors.listItem
        }

        Behavior on height { SmoothedAnimation { velocity: 500 } }
    }
}
