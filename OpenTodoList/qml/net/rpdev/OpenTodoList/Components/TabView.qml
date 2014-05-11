/*
 *  OpenTodoList - A todo and task manager
 *  Copyright (C) 2014  Martin Höher <martin@rpdev.net>
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

FocusScope {
    id: tabView

    anchors.fill: parent

    property string currentSource
    property Component currentComponent
    default property alias tabs: tabModel.children
    property alias currentTabIndex: headerBar.currentIndex
    property alias showActionButton: headerBar.showActionButton
    property alias actionButtonTitle: headerBar.actionButtonTitle

    signal actionButtonClicked()

    onTabsChanged: updateTabBar()

    function updateTabBar() {
        var tabItems = [];
        for ( var i = 0; i < tabs.length; ++i ) {
            var tab = tabs[i];
            tabItems.push( tabs[i].headerBarItem );
            tabs[i].width = Qt.binding( function() { return view.width; } );
            tabs[i].height = Qt.binding( function() { return view.height; } );
        }
        headerBar.items = tabItems;
    }

    HeaderBar {
        id: headerBar
        anchors { left: parent.left; right: parent.right; top: parent.top }

        onActionButtonClicked: tabView.actionButtonClicked()
    }

    Rectangle {
        id: shadow
        anchors { left: parent.left; right: parent.right; top: headerBar.bottom }
        height: Measures.largeBorderWidth
        gradient: Gradient {
            GradientStop {
                color: Colors.secondary1Dark1
                position: 0.0
            }
            GradientStop {
                color: Colors.window
                position: 1.0
            }
        }
    }

    VisualItemModel {
        id: tabModel
    }

    ListView {
        id: view

        anchors { left: parent.left; right: parent.right; top: shadow.bottom; bottom: parent.bottom }
        model: tabModel
        currentIndex: headerBar.currentIndex
        orientation: Qt.Horizontal
        snapMode: ListView.SnapOneItem
        highlightRangeMode: ListView.StrictlyEnforceRange
        focus: true
        clip: true
        flickDeceleration: 500
        highlightMoveDuration: 200

        onCurrentIndexChanged: {
            if ( headerBar.currentIndex !== currentIndex ) {
                headerBar.currentIndex = currentIndex;
            }
        }

    }

}
