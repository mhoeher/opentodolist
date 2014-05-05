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
import net.rpdev.OpenTodoList.Core 1.0
import net.rpdev.OpenTodoList.Components 1.0
import net.rpdev.OpenTodoList.Theme 1.0

Item {
    id: delegate

    property Todo todo

    signal clicked()

    height: childrenRect.height + Measures.tinySpace
    anchors {
        left: parent.left
        right: parent.right
        margins: Measures.smallSpace
    }

    SymbolLabel {
        id: label

        symbol: todo && todo.isDone ? Symbols.checkedBox : Symbols.uncheckedBox
        text: todo ? todo.title : ""
        placeholder: qsTr( "Untitled Todo" )
        useSymbolButton: true
        strikeOut: todo && todo.isDeleted
        anchors {
            left: parent.left
            right: menu.left
            top: parent.top
            margins: Measures.tinySpace
        }

        onClicked: delegate.clicked()
        onSymbolClicked: if ( todo ) todo.toggle()
    }

    SymbolButton {
        id: menu
        symbol: Symbols.verticalEllipsis
        color: "transparent"
        gradient: null
        border.width: 0
        fontColor: Colors.midText
        compact: true
        anchors {
            right: parent.right
            top: parent.top
            margins: Measures.tinySpace
        }

        onClicked: {
            if ( buttonBox.state === "" ) {
                buttonBox.state = "visible";
            } else {
                buttonBox.state = "";
            }
        }
    }

    Item {
        id: buttonBox

        anchors {
            left: parent.left
            right: parent.right
            top: label.bottom
            margins: Measures.tinySpace
            leftMargin: Measures.midSpace
        }
        height: 0
        clip: height < childrenRect.height
        visible: height > 0

        Column {
            id: column
            anchors {
                left: parent.left
                right: parent.right
                top: parent.top
            }

            SymbolLabel {
                symbol: Symbols.thrash
                text: qsTr( "Delete" )
                visible: todo && !todo.isDeleted
                width: parent.width
                minHeight: Measures.mHeight * 2
                onClicked: if ( todo ) todo.isDeleted = true
            }
            SymbolLabel {
                symbol: Symbols.textEditUndo
                text: qsTr( "Restore" )
                visible: todo && todo.isDeleted
                width: parent.width
                minHeight: Measures.mHeight * 2
                onClicked: if ( todo ) todo.isDeleted = false
            }
            /*SymbolLabel {
                symbol: Symbols.move
                width: parent.width
                text: qsTr( "Move" )
                minHeight: Measures.mHeight * 2
            }*/
        }


        states: State {
            name: "visible"
            PropertyChanges {
                target: buttonBox
                height: column.height
            }
        }
        transitions: Transition {
            from: ""
            to: "visible"
            reversible: true
            NumberAnimation {
                property: "height"
                duration: 200
            }
        }
    }
}
