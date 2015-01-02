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
    id: contextMenu

    property ContextMenuModel model

    function popup( contextItem, mouse ) {
        // calculate the "optimal" size for the menu:
        var optimalSize = view.optimalSize();
        view.optimalWidth = optimalSize.width;
        view.optimalHeight = optimalSize.height;

        // Find out where to position the menu:
        var mouseX = mouse ? mouse.x : 0;
        var mouseY = mouse ? mouse.y : 0;
        var absoluteX = mouseX;
        var absoluteY = mouseY;
        var element = contextItem
        while(element !== null)
        {
            absoluteX += element.x;
            absoluteY += element.y;
            element = element.parent;
        }
        view.centerX = absoluteX;
        view.centerY = absoluteY;

        show();
    }

    signal clicked( int index )

    backgroundOpacity: 0.3
    autoClose: true

    ListView {
        id: view

        property int optimalWidth: 0
        property int optimalHeight: 0
        property int centerX: 0
        property int centerY: 0

        function optimalSize() {
            var resultWidth = -1;
            var resultHeight = 0;
            var component = Qt.createComponent( "SymbolLabel.qml" );
            var m = contextMenu.model.menuEntries;

            for ( var entryIdx in contextMenu.model.menuEntries ) {
                var entry = contextMenu.model.menuEntries[ entryIdx ];
                var label = component.createObject();
                label.autoSize = true;
                label.noAnimations = true;
                label.text = entry.name;
                label.symbol = entry.symbol;
                resultWidth = Math.max( resultWidth, label.width + 4 * Measures.tinySpace );
                resultHeight += label.height * 2 + Measures.tinySpace * 2;
                label.destroy();
            }
            return { width: resultWidth, height: resultHeight };
        }

        width: optimalWidth > 0 && optimalWidth < parent.width ? optimalWidth : parent.width
        height: optimalHeight > 0 && optimalHeight < parent.height ? optimalHeight : parent.height
        x: centerX + width > parent.width ? centerX - (centerX + width - parent.width ) : centerX
        y: centerY + height > parent.height ? centerY - (centerY + height - parent.height ) : centerY
        interactive: contentHeight > height
        model: contextMenu.model.menuEntries
        clip: true

        delegate: Item {
            id: delegate

            height: label.height * 2 + Measures.tinySpace * 2
            anchors {
                left: parent.left
                right: parent.right
            }
            Rectangle {
                anchors {
                    left: parent.left
                    right: parent.right
                    top: parent.top
                }
                height: parent.height + Measures.smallBorderWidth
                border {
                    color: Colors.border
                    width: Measures.smallBorderWidth
                }
                color: Colors.listItem
            }
            SymbolLabel {
                id: label
                anchors {
                    left: parent.left
                    right: parent.right
                    verticalCenter: parent.verticalCenter
                    margins: Measures.tinySpace
                }
                text: modelData.name
                symbol: modelData.symbol
            }

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    contextMenu.clicked( index )
                    contextMenu.close()
                }
            }
        }
    }
}
