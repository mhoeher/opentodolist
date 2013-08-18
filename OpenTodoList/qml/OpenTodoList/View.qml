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
import "Utils.js" as Utils;

Rectangle {
    id: view
    x: parent.x + parent.width
    width: parent.width
    height: parent.height
    clip: true
    color: colors.window
    
    property alias toolButtons: toolbarContent.children
    property bool hidden: true
    property bool active: false
    property alias clientWidth: contentsFlickable.width
    property alias clientHeight: contentsFlickable.height
    default property alias content: contentsFlickable.children
    property string title: "View"

    onHiddenChanged: {
        if ( parent ){
            parent.activeViews = hidden ? Utils.hideView( parent.activeViews, view ) :
                                  Utils.showView( parent.activeViews, view );
        }
    }

    // Catch all mouse input when this view is active
    MouseArea {
        anchors.fill: parent
        onClicked:;
    }
    
    Rectangle {
        id: toolbar
        height: toolbarContent.childrenRect.height
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        clip: true
        color: colors.primary
        
        Flickable {
            anchors.top: parent.top
            anchors.left: parent.left
            width: parent.width
            height: childrenRect.height
            
            contentWidth: toolbarContent.childrenRect.width
            
            Row {
                id: toolbarContent
                
                //TODO: Will not work! Views remain unhidden even if they are not topmost!
                property bool disabled: !view.active
                
                Component.onCompleted: {
                    for ( var i = 0; i < children.length; ++i ) {
                        children[i].sourceIndex = i;
                    }
                }
            }
        }
    }
    
    Item {
        anchors.top: toolbar.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        
        Flickable {
            id: contentsFlickable
            anchors { fill: parent; margins: 0 }
            clip: true
            contentWidth: contentItem.childrenRect.width
            contentHeight: contentItem.childrenRect.height
        }
    }
    
    states: [
        State {
            name: "visible"
            when: view.hidden == false
            PropertyChanges {
                target: view
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
                duration: 500
                easing.type: Easing.InOutQuad
            }
        }
    ]
    
}
