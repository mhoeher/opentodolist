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
import net.rpdev.OpenTodoList.Components 1.0

Overlay {
    id: dialog

    property alias inputLabel: label.text
    property alias text: editor.text

    signal textAvailable( string text )

    function accept() {
        textAvailable( editor.text );
        close();
    }

    Rectangle {
        anchors {
            left: parent.left
            right: parent.right
            verticalCenter: parent.verticalCenter
            margins: Measures.smallSpace
        }
        height: childrenRect.height + Measures.smallSpace * 2

        Label {
            id: label
            text: qsTr( "Please input your text below:" )
            anchors {
                left: parent.left
                right: parent.right
                top: parent.top
                margins: Measures.smallSpace
            }
            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
        }

        SingleLineTextInput {
            id: editor
            placeholder: qsTr( "Input text here..." )
            anchors {
                left: parent.left
                right: parent.right
                top: label.bottom
                margins: Measures.smallSpace
            }

            onAccept: dialog.accept()        }

        Button {
            id: okButton
            text: qsTr( "OK" )
            anchors {
                right: parent.right
                top: editor.bottom
                margins: Measures.smallSpace
            }
            onClicked: dialog.accept()
        }

        Button {
            id: cancelButton
            text: qsTr ( "Cancel" )
            anchors {
                right: okButton.left
                top: editor.bottom
                margins: Measures.smallSpace
            }
            onClicked: dialog.close()
        }

    }
}
