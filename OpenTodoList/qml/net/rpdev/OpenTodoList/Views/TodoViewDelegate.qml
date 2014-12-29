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

import QtQuick 2.2
import net.rpdev.OpenTodoList.DataModel 1.0
import net.rpdev.OpenTodoList.Components 1.0
import net.rpdev.OpenTodoList.Theme 1.0

Item {
    id: delegate

    property Todo todo

    signal clicked()

    height: childrenRect.height + Measures.tinySpace * 2
    anchors {
        left: parent.left
        right: parent.right
        margins: Measures.smallSpace
    }

    Component {
        id: contextMenu

        ContextMenu {
            property Todo todo: Todo {}

            model: ContextMenuModel {
                ContextMenuEntry {
                    name: todo.isDeleted ? qsTr( "Restore" ) : qsTr( "Delete" )
                    symbol: todo.isDeleted ? Symbols.textEditUndo : Symbols.trash
                }
            }

            onClicked: {
                switch ( index ) {
                case 0:
                    todo.isDeleted = !todo.isDeleted
                    break;
                }
            }
        }
    }

    SymbolLabel {
        id: label

        symbol: todo && todo.isDone ? Symbols.checkedBox : Symbols.uncheckedBox
        text: todo ? todo.title : ""
        placeholder: qsTr( "Untitled Todo" )
        useSymbolButton: true
        anchors {
            left: parent.left
            right: menu.left
            top: parent.top
            margins: Measures.tinySpace
        }

        onClicked: delegate.clicked()
        onSymbolClicked: if ( todo ) todo.toggle()
    }

    SymbolLink {
        id: menu
        symbol: Symbols.verticalEllipsis
        minHeight: label.height
        anchors {
            right: parent.right
            top: parent.top
            margins: Measures.tinySpace
        }

        onClicked: {
            var ctxMenu = contextMenu.createObject( this );
            ctxMenu.todo.assign( delegate.todo );
            ctxMenu.popup( this );
        }
    }
}
