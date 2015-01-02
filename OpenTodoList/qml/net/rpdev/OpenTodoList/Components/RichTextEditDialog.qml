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

import QtQuick 2.0
import net.rpdev.OpenTodoList.Theme 1.0

Overlay {

    id: dialog

    property alias text: editor.text

    signal accept()

    closeOnBackPressed: false


    RichTextEditor {
        id: editor

        anchors {
            left: parent.left
            right: parent.right
            top: parent.top
            bottom: buttonBar.top
        }
    }

    Item {
        id: buttonBar

        anchors {
            left: parent.left
            right: parent.right
            bottom: parent.bottom
            margins: Measures.tinySpace
        }
        height: childrenRect.height + Measures.tinySpace

        Button {
            id: saveButton
            text: qsTr( "Save" )
            anchors {
                right: parent.right
                top: parent.top
                margins: Measures.tinySpace
            }

            onClicked: {
                dialog.accept();
                dialog.close();
            }

        }

        Button {
            id: discardButton
            text: qsTr( "Discard" )
            anchors {
                right: saveButton.left
                top: parent.top
                margins: Measures.tinySpace
            }

            onClicked: dialog.close()
        }
    }
}
