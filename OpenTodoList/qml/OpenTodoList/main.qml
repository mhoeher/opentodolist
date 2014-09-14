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
import net.rpdev.OpenTodoList.Views 1.0
import net.rpdev.OpenTodoList.Theme 1.0

Rectangle {
    id: root

    width: 800
    height: 600
    color: Colors.window

    function saveViewSettings() {
        console.debug( "SaveViewSettings" );
        settings.setValue( "OpenTodoList/ViewSettings/todoSortMode", ViewSettings.todoSortMode );
        settings.setValue( "OpenTodoList/ViewSettings/showDoneTodos", ViewSettings.showDoneTodos );
        settings.setValue( "OpenTodoList/ViewSettings/showDeletedTodos", ViewSettings.showDeletedTodos );
    }

    onFocusChanged: {
        // required to give back focus to the page stack whenever a
        // overlay has been displayed
        if ( focus ) {
            pageStack.focus = true
        }
    }

    Component.onCompleted: {
        width = settings.getValue( "OpenTodoList/Window/width", width );
        height = settings.getValue( "OpenTodoList/Window/height", height );
        ViewSettings.todoSortMode = settings.getValue( "OpenTodoList/ViewSettings/todoSortMode", ViewSettings.todoSortMode );
        ViewSettings.showDoneTodos = settings.getValue( "OpenTodoList/ViewSettings/showDoneTodos", ViewSettings.showDoneTodos ) === "true";
        ViewSettings.showDeletedTodos = settings.getValue( "OpenTodoList/ViewSettings/showDeletedTodos", ViewSettings.showDeletedTodos ) === "true";

        ViewSettings.onSettingsChanged.connect( saveViewSettings );
    }

    onWidthChanged: settings.setValue( "OpenTodoList/Window/width", width )
    onHeightChanged: settings.setValue( "OpenTodoList/Window/height", height )

    TodoListLibrary {
        id: todoListLibrary
    }

    Settings {
        id: settings
    }

    PageStack {
        id: pageStack

        anchors.fill: parent
        focus: true

        onLastPageClosing: {
            if ( Qt.os === "android" ) {
                commandHandler.hideWindow();
            }
        }

        TodoListsPage {
            library: todoListLibrary

            onTodoSelected: {
                var component = Qt.createComponent( "TodoPage.qml" );
                if ( component.status === Component.Ready ) {
                    var page = component.createObject( pageStack );
                    page.todo.shadow( todo );
                    pageStack.showPage( page );
                } else {
                    console.error( component.errorString() );
                }
            }
        }
    }
}
