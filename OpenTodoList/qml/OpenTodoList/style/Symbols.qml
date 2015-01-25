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

pragma Singleton
import QtQuick 2.2

QtObject {
    readonly property string deleteText: "\uf057"

    readonly property string uncheckedBox: "\uf096"
    readonly property string checkedBox: "\uf046"

    readonly property string doubleLeft: "\uf100"
    readonly property string doubleRight: "\uf101"
    readonly property string doubleUp: "\uf102"
    readonly property string doubleDown: "\uf103"
    readonly property string singleLeft: "\uf104"
    readonly property string singleRight: "\uf105"
    readonly property string singleUp: "\uf106"
    readonly property string singleDown: "\uf107"

    readonly property string textEditBold: "\uf032"
    readonly property string textEditItalic: "\uf033"
    readonly property string textEditUnderline: "\uf0cd"
    readonly property string textEditStrikethrough: "\uf0cc"
    readonly property string textEditAlignLeft: "\uf036"
    readonly property string textEditCenter: "\uf037"
    readonly property string textEditAlignRight: "\uf038"
    readonly property string textEditJustify: "\uf039"
    readonly property string textEditCopy: "\uf0c5"
    readonly property string textEditCut: "\uf0c4"
    readonly property string textEditPaste: "\uf0ea"
    readonly property string textEditUnorderedList: "\uf0ca"
    readonly property string textEditOrderedList: "\uf0cb"
    readonly property string textEditUndo: "\uf0e2"
    readonly property string textEditRedo: "\uf01e"

    readonly property string calendarFull: "\uf073"
    readonly property string calendarEmpty: "\uf133"

    readonly property string verticalEllipsis: "\uf142"
    readonly property string horizontalEllipsis: "\uf141"
    readonly property string bars: "\uf0c9"


    readonly property string trash: "\uf014"
    readonly property string move: "\uf047"

    readonly property string plus: "\uf067"
    readonly property string minus: "\uf068"

    readonly property string sortByName: "\uf15d"
    readonly property string sortByValue: "\uf160"
    readonly property string exclamationMark: "\uf12a"
    readonly property string tasks: "\uf0ae"

    readonly property string search: "\uf002"
    readonly property string close: "\uf00d"

    readonly property string eye: "\uf06e"
    readonly property string eyeSlash: "\uf070"
}
