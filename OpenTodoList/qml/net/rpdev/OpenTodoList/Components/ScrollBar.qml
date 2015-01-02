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

Item {
    id: scrollBar

    property Flickable target: null

    readonly property double xWidth: {
        if ( target && target.contentWidth > target.width && target.contentWidth > 0 ) {
            var result = target.width / target.contentWidth * width;
            result = Math.max( Measures.mHeight * 2, result );
            result = Math.min( width, result );
            return result;
        }
        return width - scrollBarTotalWidth;
    }

    readonly property double xOffset: {
        if ( target && target.contentWidth != target.width ) {
            return target.contentX / ( target.contentWidth - width ) * (width - xWidth - scrollBarTotalWidth);
        }
        return 0;
    }

    readonly property double yHeight: {
        if ( target && target.contentHeight > target.height && target.contentHeight > 0 ) {
            var result = target.height / target.contentHeight * height;
            result = Math.max( Measures.mHeight * 2, result );
            result = Math.min( height, result );
            return result;
        }
        return height - scrollBarTotalWidth;
    }

    readonly property double yOffset: {
        if ( target && target.contentHeight != target.height ) {
            return target.contentY / ( target.contentHeight - target.height ) * (height - yHeight - scrollBarTotalWidth);
        }
        return 0;
    }

    readonly property int scrollBarMargin: Measures.tinySpace
    readonly property int scrollBarWidth: Measures.tinySpace
    readonly property int scrollBarTotalWidth: scrollBarMargin + scrollBarWidth

    onXWidthChanged: autoHideBottomScrollBar.show()
    onXOffsetChanged: autoHideBottomScrollBar.show()
    onYHeightChanged: autoHideRightScrollBar.show()
    onYOffsetChanged: autoHideRightScrollBar.show()

    anchors.fill: target
    clip: true

    Rectangle {
        id: bottomScrollBar

        anchors.bottom: parent.bottom
        anchors.margins: scrollBarMargin
        x: xOffset
        width: xWidth
        height: scrollBarWidth
        color: Colors.secondary1
        border {
            width: Measures.smallBorderWidth
            color: Colors.activeButton
        }
        radius: Measures.tinySpace
        opacity: 0.0
        visible: opacity > 0

        Behavior on opacity { NumberAnimation { duration: 100 } }
    }

    AutoHide {
        id: autoHideBottomScrollBar
        maxStayVisibleTime: 1000
        target: bottomScrollBar
    }

    Rectangle {
        id: rightScrollBar

        anchors.right: parent.right
        anchors.margins: scrollBarMargin
        y: yOffset
        height: yHeight
        width: scrollBarWidth
        color: Colors.secondary1
        border {
            width: Measures.smallBorderWidth
            color: Colors.activeButton
        }
        radius: Measures.tinySpace
        opacity: 0.0
        visible: opacity > 0

        Behavior on opacity { NumberAnimation { duration: 100 } }
    }

    AutoHide {
        id: autoHideRightScrollBar
        maxStayVisibleTime: autoHideBottomScrollBar.maxStayVisibleTime
        target: rightScrollBar
    }
}
