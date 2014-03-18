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
import net.rpdev.OpenTodoList.Core 1.0
import "../controls"

View {
    id: deletedTodosView

    property QtObject todoList: null
    property TodoSortFilterModel model: TodoSortFilterModel {
        sourceModel: todoList ? todoList.todos : null
        filterMode: TodoSortFilterModel.HideNonDeleted
    }

    hidden: !todoList

    toolButtons: [
        ToolButton {
            symbol: "\uf060"
            isBackButton: true

            onClicked: deletedTodosView.todoList = null
        },
        ToolButton {
            id: emptyTrashButton
            symbol: "\uf014"
            visible: model.count > 0

            onClicked: {
                if ( model.count > 0 ) {
                    var options = [
                                { label: qsTr( "Empty Trash"), id: "emptyTrash" },
                                { label: qsTr( "Cancel" ), id: "cancel" }
                            ];
                    popup.result.connect( emptyTrashButton.handleResult )
                    popup.show( options )
                }
            }

            function handleResult( result ) {
                popup.result.disconnect( emptyTrashButton.handleResult )
                if ( result === "emptyTrash" ) {
                    var todos = [];
                    for ( var i = 0; i < model.count; ++i ) {
                        todos.push( model.getItem( i ).object );
                    }
                    for ( var i = 0; i < todos.length; ++i ) {
                        todos[ i ].dispose();
                    }
                }
            }
        }

    ]

    TodoView {
        width: deletedTodosView.clientWidth
        height: deletedTodosView.clientHeight
        model: deletedTodosView.model
    }
}
