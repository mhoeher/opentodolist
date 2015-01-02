/*
 *  OpenTodoList - A todo and task manager
 *  Copyright (C) 2014 - 2015 Martin Höher <martin@rpdev.net>
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
import net.rpdev.OpenTodoList.SystemIntegration 1.0
import net.rpdev.OpenTodoList.Theme 1.0

Rectangle {
    width: 800
    height: 600

    property alias text: editor.text

    Rectangle {
        id: toolBar

        color: Colors.primary

        anchors {
            top:  parent.top
            left: parent.left
            right: parent.right
        }
        height: childrenRect.height

        Flickable {
            anchors {
                left: parent.left
                right: parent.right
                top: parent.top
                margins: Measures.tinySpace
            }
            height: contentItem.childrenRect.height + 2 * Measures.tinySpace
            contentWidth: contentItem.childrenRect.width

            Row {
                height: childrenRect.height
                spacing: Measures.tinySpace

                SymbolButton {
                    id: copyButton
                    symbol: Symbols.textEditCopy
                    onClicked: editor.copy()
                }
                SymbolButton {
                    id: cutButton
                    symbol: Symbols.textEditCut
                    onClicked: editor.cut()
                }
                SymbolButton {
                    id: pasteButton
                    symbol: Symbols.textEditPaste
                    onClicked: editor.paste()
                }

                Item {
                    width: Measures.tinySpace
                    height: 1
                }

                SymbolButton {
                    id: undoButton
                    symbol: Symbols.textEditUndo
                    onClicked: editor.undo()
                }
                SymbolButton {
                    id: redoButton
                    symbol: Symbols.textEditRedo
                    onClicked: editor.redo()
                }

                Item {
                    width: Measures.tinySpace
                    height: 1
                }

                SymbolButton {
                    id: boldButton
                    symbol: Symbols.textEditBold
                    checked: formatter.bold
                    onClicked: formatter.bold = !formatter.bold
                }
                SymbolButton {
                    id: italicButton
                    symbol: Symbols.textEditItalic
                    checked: formatter.italic
                    onClicked: formatter.italic = !formatter.italic
                }
                SymbolButton {
                    id: underlineButton
                    symbol: Symbols.textEditUnderline
                    checked: formatter.underline
                    onClicked: formatter.underline = !formatter.underline
                }
                SymbolButton {
                    id: strikethroughButton
                    symbol: Symbols.textEditStrikethrough
                    checked: formatter.strikethrough
                    onClicked: formatter.strikethrough = !formatter.strikethrough
                }

                Item {
                    width: Measures.tinySpace
                    height: 1
                }

                SymbolButton {
                    id: alignLeftButton
                    symbol: Symbols.textEditAlignLeft
                    checked: formatter.alignLeft
                    onClicked: formatter.alignLeft = true
                }
                SymbolButton {
                    id: alignCenterButton
                    symbol: Symbols.textEditCenter
                    checked: formatter.alignCenter
                    onClicked: formatter.alignCenter = true
                }
                SymbolButton {
                    id: alignRightButton
                    symbol: Symbols.textEditAlignRight
                    checked: formatter.alignRight
                    onClicked: formatter.alignRight = true
                }
                SymbolButton {
                    id: justifyTextButton
                    symbol: Symbols.textEditJustify
                    checked: formatter.justify
                    onClicked: formatter.justify = true
                }

                Item {
                    width: Measures.tinySpace
                    height: 1
                }

                SymbolButton {
                    id: unorderedListButton
                    symbol: Symbols.textEditUnorderedList
                    checked: formatter.unorderedList
                    onClicked: formatter.unorderedList = !formatter.unorderedList
                }
                SymbolButton {
                    id: orderedListButton
                    symbol: Symbols.textEditOrderedList
                    checked: formatter.orderedList
                    onClicked: formatter.orderedList = !formatter.orderedList
                }
            }
        }
    }

    Rectangle {
        anchors.fill: editorFlickable
        color: Colors.window
    }

    Flickable {
         id: editorFlickable

         anchors {
             top: toolBar.bottom
             left: parent.left
             right: parent.right
             bottom: parent.bottom
         }
         contentWidth: editor.paintedWidth
         contentHeight: editor.paintedHeight
         clip: true

         function ensureVisible(rect)
         {
             if (contentX >= rect.x)
                 contentX = rect.x;
             else if (contentX+width <= rect.x+rect.width)
                 contentX = rect.x+rect.width-width;
             if (contentY >= rect.y)
                 contentY = rect.y;
             else if (contentY+height <= rect.y+rect.height)
                 contentY = rect.y+rect.height-height;
         }

         TextEditor {
             id: editor
             wrapMode: TextEdit.WrapAtWordBoundaryOrAnywhere
             width: editorFlickable.width
             height: Math.max( paintedHeight, editorFlickable.height )
             textFormat: TextEdit.RichText
             onCursorRectangleChanged: editorFlickable.ensureVisible(cursorRectangle)
             focus: true
         }
     }

    Shadow {
        anchors {
            top: toolBar.bottom
            left: parent.left
            right: parent.right
        }
    }

    DocumentFormatter {
        id: formatter

        target: editor
        selectionStart: editor.selectionStart
        selectionEnd: editor.selectionEnd
        cursorPosition: editor.cursorPosition
    }

    ScrollBar {
        target: editorFlickable
    }

    Shortcut {
        keySequence: fromStandardKey( StandardKey.Bold )
        onTriggered: formatter.bold = !formatter.bold
    }

    Shortcut {
        keySequence: fromStandardKey( StandardKey.Italic )
        onTriggered: formatter.italic = !formatter.italic
    }

    Shortcut {
        keySequence: fromStandardKey( StandardKey.Underline )
        onTriggered: formatter.underline = !formatter.underline
    }
}
