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
import QtQuick.Layouts 1.0
import "Utils.js" as Utils;

Item {
    id: entry
    
    property QtObject todo: null
    property alias containsMouse: mouseArea.containsMouse
    property color fontColor: colors.fontColorFor( color )
    property int padding: 4
    property bool hasNext: true
    
    signal clicked

    //color: colors.button
    width: parent.width
    height: childrenRect.height + 2*padding

    MouseArea {
        id: mouseArea
        anchors.fill: entryContent
        hoverEnabled: true
        
        onClicked: entry.clicked()
    }

    RowLayout {
        id: entryContent
        anchors.left: parent.left
        anchors.leftMargin: 4
        anchors.right: parent.right
        anchors.rightMargin: 4
        y: padding

        Item {
            width: childrenRect.width
            height: childrenRect.height
            anchors.verticalCenter: parent.verticalCenter
            SymbolButton {
                text: "\uf0c8"
                color: Utils.PriorityColors[entry.todo.priority]
            }
            SymbolButton {
                id: checkMark
                text: entry.todo.progress === 100 ? "\uf046" : "\uf096"
                color: fontColor
                onClicked: entry.todo.progress = ( entry.todo.progress === 100 ? 0 : 100 )
            }
        }

        Text {
            id: label
            text: entry.todo.title
            color: fontColor
            font.pointSize: 12
            wrapMode: Text.Wrap
            Layout.fillWidth: true
        }

        SymbolButton {
            id: deleteTodoButton
            text: entry.todo.deleted ? "\uf0e2" : "\uf014"
            color: fontColor
            onClicked: entry.todo.deleted = !entry.todo.deleted;
        }
    }
    Rectangle {
        height: entry.hasNext ? 2 : 0
        border.color: "#50000000"
        anchors { left: parent.left; right: parent.right; top: entryContent.bottom; topMargin: entry.padding }
    }
}
