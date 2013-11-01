/*
 *  OpenTodoList - A todo and task manager
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
import "../../../js/Utils.js" as Utils;

Rectangle {
    id: entry
    
    property QtObject todo: null
    property alias containsMouse: mouseArea.containsMouse
    property color fontColor: "black"
    property int padding: 4
    property bool hasNext: true

    readonly property string checkActionApplyAll: "all"
    readonly property string checkActionShowDetails: "details"
    readonly property string checkActionCancel: "cancel"
    
    signal clicked

    height: childrenRect.height + 2*padding
    color: "transparent"
    border.color: entry.todo.priority >= 0 ? priorityHint.color : "gray"

    MouseArea {
        id: mouseArea
        anchors.fill: entryContent
        hoverEnabled: true
        
        onClicked: entry.clicked()
    }

    Image {
        id: priorityHint

        property color color: Utils.colorForPriority( entry.todo.priority )
        property int size: layout.minimumButtonHeight / 4

        visible: entry.todo.priority >= 0
        source:"image://primitives/polygon/color=" + color +
               ",fill=" + color +
               ",points=0:0-1:0-1:1"
        width: size
        height: size
        anchors.right: parent.right
        anchors.top: parent.top
    }

    RowLayout {
        id: entryContent
        anchors.left: parent.left
        anchors.leftMargin: + priorityHint.width
        anchors.right: parent.right
        anchors.rightMargin: 4 + priorityHint.width
        spacing: 10
        y: padding

        Item {
            width: childrenRect.width
            height: childrenRect.height
            anchors.verticalCenter: parent.verticalCenter
            SymbolButton {
                id: checkMark

                function handleResult( result ) {
                    if ( result === checkActionApplyAll ) {
                        Utils.recursiveSetTodoProgress( entry.todo, entry.todo.progress === 100 ? 0 : 100 )
                    } else if ( result === checkActionShowDetails ) {
                        entry.clicked();
                    }
                    popup.result.disconnect( handleResult );
                }

                text: entry.todo.progress === 100 ? "\uf046" : "\uf096"
                color: fontColor

                onClicked: if ( entry.todo.hasSubTodos() ) {
                               var options = [
                                           { label: entry.todo.progress === 100 ? "Uncheck Sub Todos" : "Check Sub Todos", id: checkActionApplyAll },
                                           { label: "Show Details", id: checkActionShowDetails },
                                           { label: "Cancel", id: checkActionCancel }
                                       ];
                               popup.result.connect( checkMark.handleResult )
                               popup.show( options )
                           } else {
                               entry.todo.progress =
                                       ( entry.todo.progress === 100 ? 0 : 100 )
                           }
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
}
