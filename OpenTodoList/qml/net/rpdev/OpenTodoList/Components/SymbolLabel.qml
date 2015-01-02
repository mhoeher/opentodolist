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
import QtQuick.Layouts 1.1
import net.rpdev.OpenTodoList.Theme 1.0
import net.rpdev.OpenTodoList.Components 1.0

Item {
    id: component

    property alias symbol: symbolLabel.symbol
    property alias text: label.text
    property alias placeholder: placeholderLabel.text
    property bool useSymbolButton: false
    property alias strikeOut: label.font.strikeout
    property int minHeight: 0
    property bool autoSize: false
    property bool noAnimations: false

    signal symbolClicked()
    signal clicked()

    height: Math.max(
                symbolLabel.height,
                label.height,
                placeholderLabel.height,
                minHeight )
    width: autoSize ? symbolLabel.width + Math.max( label.width, placeholderLabel.width ) + Measures.smallSpace : 0

    MouseArea {
        anchors.fill: parent
        onClicked: component.clicked()
    }

    Symbol {
        id: symbolLabel

        anchors {
            left: parent.left
            verticalCenter: parent.verticalCenter
        }

        Behavior on text {
            enabled: !noAnimations
            SequentialAnimation {
                NumberAnimation {
                    target: symbolLabel
                    property: "opacity"
                    to: 0.0
                    duration: 100
                }
                PropertyAnimation {
                    duration: 50
                }
                NumberAnimation {
                    target: symbolLabel
                    property: "opacity"
                    to: 1.0
                    duration: 100
                }
            }
        }
    }

    MouseArea {
        enabled: useSymbolButton
        anchors.fill: symbolLabel
        onClicked: component.symbolClicked()
    }

    Label {
        id: label

        anchors {
            left: symbolLabel.right
            right: autoSize ? undefined : parent.right
            leftMargin: Measures.smallSpace
            verticalCenter: parent.verticalCenter
        }
        text: qsTr( "SymbolLabel" )
        wrapMode: Text.WrapAtWordBoundaryOrAnywhere
        autoSize: component.autoSize
    }

    Label {
        id: placeholderLabel

        anchors {
            left: symbolLabel.right
            right: autoSize ? undefined : parent.right
            leftMargin: Measures.smallSpace
            verticalCenter: parent.verticalCenter
        }
        text: ""
        color: Colors.midText
        font.italic: true
        wrapMode: Text.WrapAtWordBoundaryOrAnywhere
        autoSize: component.autoSize
        visible: label.text === ""
        font {
            strikeout: label.font.strikeout
        }
    }
}
