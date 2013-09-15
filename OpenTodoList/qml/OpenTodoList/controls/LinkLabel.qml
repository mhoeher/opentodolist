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

Item {
    id: label

    property alias label: text.text
    property alias font: text.font
    property color color: colors.complementaryColorLighter2
    property color activeColor: colors.primaryLighter2
    property bool active: false
    property bool editable: false

    signal clicked
    signal editFinished

    width: 100
    height: flow.height

    onEditFinished: text.focus = false

    MouseArea {
        id: mouseArea

        anchors.fill: flow
        cursorShape: Qt.PointingHandCursor

        onClicked: label.editable ? text.focus = true : label.clicked()
    }

    Row {
        id: flow
        width: parent.width

        TextInput {
            id: text

            font.pointSize: fonts.h3
            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
            width: parent.width - ( label.editable ? saveButton.width - flow.spacing : 0 )
            color: label.active ? label.activeColor : label.color
            enabled: label.editable

            Keys.onEnterPressed: label.editFinished()
            Keys.onReturnPressed: label.editFinished()
            Keys.onEscapePressed: text.focus = false
            Keys.onBackPressed: text.focus = fase

            Behavior on color { ColorAnimation { duration: 200 } }
        }

        Button {
            id: saveButton

            label: "Save"
            opacity: label.editable && text.focus ? 1 : 0
            visible: opacity > 0

            onClicked: label.editFinished()

            Behavior on opacity { NumberAnimation { duration: 200 } }
        }

        Behavior on height { SmoothedAnimation { duration: 200 } }
    }
}
