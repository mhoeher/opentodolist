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
import "helper"
import "views"
import "controls"

ViewContainer {
    id: root
    width: 800
    height: 600

    Layout {
        id: layout
    }

    ColorScheme {
        id: colors
    }

    FontLayout {
        id: fonts
    }

    FontLoader {
        id: symbolFont
        source: "res/fontawesome-webfont.ttf"
    }
    
    TodoListView {
        id: todoListView

        title: "Todo Lists"
        
        onTodoSelected: {
            todoDetailsView.todo = todo;
        }
        onTodoListClicked: todoListContentsView.showIfCompactView = true
        onShowTrashForList: deletedTodosView.todoList = list
        onShowSearch: searchView.hidden = false
        Component.onCompleted: hidden = false
    }

    TodoListContentsView {
        id: todoListContentsView

        onTodoSelected: {
            todoDetailsView.todo = todo;
        }
        hidden: !(showIfCompactView && layout.useCompactLayout)
    }
    
    NewTodoListView {
        id: newTodoListView

        title: "New Todo List"
    }

    SearchView {
        id: searchView

        title: "Search"
        hidden: true

        onTodoSelected: todoDetailsView.todo = todo
    }
       
    TodoDetailsView {
        id: todoDetailsView

        title: "Todo"
    }

    DeletedTodosView {
        id: deletedTodosView
    }
    
    Popup {
        id: popup
    }
}

