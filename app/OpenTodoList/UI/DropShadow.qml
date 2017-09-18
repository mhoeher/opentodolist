import QtQuick 2.0

import OpenTodoList.UI 1.0

Rectangle {
    id: shadow
    
    property Item sourceItem
    property bool hovered: false
    property int borderWidth: Globals.defaultMargin / 2
    property int hoverBorderWidth: Globals.defaultMargin / 3 * 2
    
    property int __borderWidth: hovered ? hoverBorderWidth : borderWidth
    
    color: "black"
    opacity: hovered ? 0.6 : 0.4
    anchors {
        fill: sourceItem
        leftMargin: __borderWidth
        topMargin: __borderWidth
        rightMargin: -__borderWidth
        bottomMargin: -__borderWidth
    }
    
    Behavior on opacity {
        SmoothedAnimation {
            duration: Globals.defaultAnimationTime
        }
    }
    
    Behavior on __borderWidth {
        SmoothedAnimation {
            duration: Globals.defaultAnimationTime
        }
    }
}
