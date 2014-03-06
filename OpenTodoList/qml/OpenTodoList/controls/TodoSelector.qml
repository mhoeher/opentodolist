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
import QtQuick.Layouts 1.0

Item {
    id: root
    width: 100
    height: 200

    property QtObject selectedObject: null
    property QtObject currentObject: null


    function show() {
        view.delegate = null;
        view.model = library.todoLists;
        view.delegate = todoListDelegate;
    }

    function browseTodoList( todoList ) {
        currentObject = todoList;
        view.delegate = null;
        view.model = todoList.entries;
        view.delegate = todoDelegate;
    }

    function browseTodo( todo ) {
        currentObject = todo;
        view.delegate = null;
        view.model = todo.subTodos;
        view.delegate = todoDelegate;
    }

    function browseParent() {
        if ( currentObject ) {
            if ( currentObject.parentTodo ) {
                browseTodo( currentObject.parentTodo );
            } else if ( currentObject.todoList ) {
                browseTodoList( currentObject.todoList );
            } else {
                show();
            }
        }
    }

    Component.onCompleted: show()

    Component {
        id: todoListDelegate

        RowLayout {
            width: parent.width
            height: childrenRect.height

            Button {
                id: selectButton
                label: object.name
                Layout.fillWidth: true
                down: root.selectedObject === object

                onClicked: root.selectedObject = object
            }
            LinkLabel {
                id: goIntoLabel
                label: "\uf061"
                width: layout.minimumButtonHeight

                onClicked: root.browseTodoList( object )
            }
        }
    }

    Component {
        id: todoDelegate

        RowLayout {
            width: parent.width
            height: childrenRect.height
            visible: !object.deleted
            Button {
                id: selectButton
                label: object.title
                Layout.fillWidth: true
                down: root.selectedObject === object

                onClicked: root.selectedObject = object
            }
            LinkLabel {
                id: goIntoLabel
                label: "\uf061"
                visible: object.numTodos > 0
                width: layout.minimumButtonHeight

                onClicked: root.browseTodo( object )
            }
        }
    }

    Button {
        id: goUpButton
        width: parent.width - 2 * anchors.margins
        x: root.currentObject ? anchors.margins : parent.width
        label: qsTr( "Go up" )
        color: colors.complementaryColor
        highlightColor: colors.complementaryColorLighter1
        anchors {
            top: parent.top
            margins: 10
        }

        onClicked: root.browseParent()
    }

    ListView {
        id: view
        clip: true
        anchors {
            top: root.currentObject ? goUpButton.bottom : parent.top
            left: parent.left
            right: parent.right
            bottom: parent.bottom
            margins: 10
        }
        spacing: 10
    }
}
