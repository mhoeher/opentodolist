/*
 *  OpenTodoList - A todo and task manager
 *  Copyright (C) 2014 - 2015  Martin Höher <martin@rpdev.net>
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

import QtQuick 2.4
import QtQuick.Controls 1.2
import net.rpdev.OpenTodoList.Core 1.0
import net.rpdev.OpenTodoList.SystemIntegration 1.0
import "../style" as Style

Rectangle {
    id: toolBar

    property TextArea editor: null
    color: Style.Colors.primaryLight2

    anchors {
        top:  parent.top
        left: parent.left
        right: parent.right
    }
    height: childrenRect.height

    DocumentFormatter {
        id: formatter
        target: editor
        selectionStart: editor ? editor.selectionStart : 0
        selectionEnd: editor ? editor.selectionEnd : 0
    }

    Action {
        shortcut: StandardKey.Bold
        onTriggered: formatter.bold = !formatter.bold
    }
    Action {
        shortcut: StandardKey.Italic
        onTriggered: formatter.italic = !formatter.italic
    }
    Action {
        shortcut: StandardKey.Underline
        onTriggered: formatter.underline = !formatter.underline
    }

    Flow {
        //height: childrenRect.height
        width: parent.width
        flow: Flow.LeftToRight
        //spacing: Style.Measures.tinySpace

        Symbol {
            id: undoButton
            symbol: Style.Symbols.textEditUndo
            onClicked: editor.undo()
        }
        Symbol {
            id: redoButton
            symbol: Style.Symbols.textEditRedo
            onClicked: editor.redo()
        }

        Item {
            width: Style.Measures.tinySpace
            height: 1
        }

        Symbol {
            id: boldButton
            symbol: Style.Symbols.textEditBold
            checked: formatter.bold
            onClicked: formatter.bold = !formatter.bold
        }
        Symbol {
            id: italicButton
            symbol: Style.Symbols.textEditItalic
            checked: formatter.italic
            onClicked: formatter.italic = !formatter.italic
        }
        Symbol {
            id: underlineButton
            symbol: Style.Symbols.textEditUnderline
            checked: formatter.underline
            onClicked: formatter.underline = !formatter.underline
        }
        Symbol {
            id: strikethroughButton
            symbol: Style.Symbols.textEditStrikethrough
            checked: formatter.strikethrough
            onClicked: formatter.strikethrough = !formatter.strikethrough
        }

        Item {
            width: Style.Measures.tinySpace
            height: 1
        }

        Symbol {
            id: alignLeftButton
            symbol: Style.Symbols.textEditAlignLeft
            checked: formatter.alignLeft
            onClicked: formatter.alignLeft = true
        }
        Symbol {
            id: alignCenterButton
            symbol: Style.Symbols.textEditCenter
            checked: formatter.alignCenter
            onClicked: formatter.alignCenter = true
        }
        Symbol {
            id: alignRightButton
            symbol: Style.Symbols.textEditAlignRight
            checked: formatter.alignRight
            onClicked: formatter.alignRight = true
        }
        Symbol {
            id: justifyTextButton
            symbol: Style.Symbols.textEditJustify
            checked: formatter.justify
            onClicked: formatter.justify = true
        }

        Item {
            width: Style.Measures.tinySpace
            height: 1
        }

        Symbol {
            id: unorderedListButton
            symbol: Style.Symbols.textEditUnorderedList
            checked: formatter.unorderedList
            onClicked: formatter.unorderedList = !formatter.unorderedList
        }
        Symbol {
            id: orderedListButton
            symbol: Style.Symbols.textEditOrderedList
            checked: formatter.orderedList
            onClicked: formatter.orderedList = !formatter.orderedList
        }
    }
}
