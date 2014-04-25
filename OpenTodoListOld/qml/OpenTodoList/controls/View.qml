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
import "../js/Utils.js" as Utils;

FocusScope {
    id: component

    x: parent.x + parent.width
    width: parent.width
    height: parent.height
    focus: active

    signal stateChangeFinished()

    property alias toolButtons: toolbar.children
    property bool hidden: true
    property bool active: false
    property alias clientWidth: contents.width
    property alias clientHeight: contents.height
    default property alias content: contents.children
    property string title: "View"
    property int padding: 20

    readonly property int stateChangeTime: 500

    function hideAndDestroy() {
        hidden = true;
        destroy( stateChangeTime );
    }

    onFocusChanged: view.focus = true

    onHiddenChanged: {
        if ( parent ){
            parent.activeViews = hidden ? Utils.hideView( parent.activeViews, component ) :
                                          Utils.showView( parent.activeViews, component );
        }
    }

    MouseArea {
        anchors.fill: parent
        cursorShape: Qt.ArrowCursor
    }

    Rectangle {
        id: view
        anchors.fill: parent
        clip: true
        color: colors.window
        focus: true

        Keys.onReleased: {
            if ( event.key === Qt.Key_Escape || event.key === Qt.Key_Back ) {
                for ( var i = 0; i < component.toolButtons.length; ++i ) {
                    var toolButton = component.toolButtons[i];
                    if ( toolButton.isBackButton ) {
                        toolButton.clicked();
                        event.accepted = true;
                        return;
                    }
                }
            }
        }

        Rectangle {
            id: toolbarOuter
            color: colors.primary
            height: childrenRect.height
            clip: true
            radius: 5
            anchors { left: parent.left; right: parent.right; top: parent.top
                leftMargin: padding; rightMargin: padding; topMargin: padding }
            Flickable {
                clip: true
                anchors { left: parent.left; right: parent.right;
                    margins: parent.radius }
                height: toolbar.childrenRect.height
                contentWidth: toolbar.childrenRect.width
                contentHeight: toolbar.childrenRect.height
                Row {
                    id: toolbar
                    anchors.left: parent.left
                    anchors.leftMargin: spacing / 2
                }
            }
        }

        Item {
            id: toolbarDecorationOuter
            anchors { left: parent.left; right: parent.right; top: toolbarOuter.bottom;
                leftMargin: padding * 2; rightMargin: padding }
            height: childrenRect.height
            Item {
                width: 30
                height: 30
                Image {
                    sourceSize.width: width
                    sourceSize.height: height
                    anchors.fill: parent
                    source:"image://primitives/polygon/color=" + colors.primary +
                           ",fill=" + colors.primary +
                           ",points=0:0-15:30-30:0"
                }
            }
        }

        Rectangle {
            id: contents
            color: colors.view
            clip: true

            anchors { left: parent.left; right: parent.right;
                top: toolbarDecorationOuter.bottom; bottom: parent.bottom;
                leftMargin: padding; rightMargin: padding; bottomMargin: padding }
        }

    }

    states: [
        State {
            name: "visible"
            when: component.hidden == false
            PropertyChanges {
                target: component
                x: 0
            }
        }
    ]

    transitions: [
        Transition {
            from: ""
            to: "visible"
            reversible: true
            NumberAnimation {
                properties: "x"
                duration: stateChangeTime
                easing.type: Easing.InOutQuad
            }
        }
    ]
}
