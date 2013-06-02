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
    id: view
    x: parent.x + parent.width
    width: parent.width
    height: parent.height
    clip: true
    
    property alias toolButtons: toolbarContent.children
    property bool hidden: true
    property alias clientWidth: contentsFlickable.width
    property alias clientHeight: contentsFlickable.height
    default property alias content: contentPanel.children
    
    Item {
        id: toolbar
        height: toolbarContent.childrenRect.height
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        
        Flickable {
            anchors.top: parent.top
            anchors.left: parent.left
            width: parent.width
            height: childrenRect.height
            
            contentWidth: toolbarContent.childrenRect.width
            
            Row {
                id: toolbarContent
                x: parent.x + 5
                y: parent.y + 5
                spacing: 3
            }
        }
    }
    
    Item {
        anchors.top: toolbar.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.topMargin: 10
        
        BorderImage {
            anchors.fill: parent
            
            source: "view.sci"
            //TODO: Remove this, this is for debugging only!
            cache: false
            smooth: true
            
            Component.onCompleted: rotation = 3 * Math.random() - 1.5
        }
        
        Flickable {
            id: contentsFlickable
            anchors { fill: parent; margins: 20 }
            clip: true
            contentWidth: contentPanel.width
            contentHeight: contentPanel.height
            
            Item {
                id: contentPanel
                
                width: childrenRect.width
                height: childrenRect.height
            }
        }
    }
    
    states: [
        State {
            name: "visible"
            when: view.hidden == false
            PropertyChanges {
                target: view
                x: view.parent.x
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
                duration: 500
                easing.type: Easing.InOutQuad
            }
        }
    ]
    
}