import QtQuick 2.5

import net.rpdev.OpenTodoList.UI 1.0


Item {
    id: panel
    
    property int edge: Qt.LeftEdge
    property bool compact: false
    property bool showing: !compact
    property Component contentItem: Item{}
    
    property Item __parent: (parent && compact) ? parent : panel
    
    function close() {
        if (compact && showing) {
            showing = false;
        }
    }
    
    x: !compact ? 0 : -width
    
    MouseArea {
        //visible: panel.compact && panel.showing
        enabled: panel.showing
        hoverEnabled: true
        
        width: panel.__parent.width
        height: panel.__parent.height
        x: panel.width
        
        onClicked: panel.showing = false
        
        Rectangle {
            anchors.fill: parent
            color: "black"
            opacity: {
                var result = 0.0;
                if (panel.compact) {
                    if (panel.showing) {
                        result = (contents.x > 0) ? ((contents.x) / panel.width) : 1.0;
                    } else {
                        result = 0.0;
                    }
                }
                return result * 0.7;
            }
        }
    }
   
    Rectangle {
        width: {
            switch (edge) {
            case Qt.TopEdge:
            case Qt.BottomEdge:
                return parent.width
            case Qt.LeftEdge:
            case Qt.RightEdge:
                return parent.height
            }
        }
        height: {
            switch (edge) {
            case Qt.TopEdge:
            case Qt.BottomEdge:
                return parent.height
            case Qt.LeftEdge:
            case Qt.RightEdge:
                return parent.width
            }
        }
        anchors.centerIn: contents
        rotation: {
            switch (edge) {
            case Qt.LeftEdge: return 270;
            case Qt.RightEdge: return 90;
            case Qt.TopEdge: return 360;
            case Qt.BottomEdge: return 180;
            default:
                console.error("Bad edge: " + edge)
                return 0;
            }
        }
        gradient: Gradient {
            GradientStop {
                position: 0.0
                color: Colors.panelDark
            }
            GradientStop {
                position: 1.0
                color: Colors.panelLight
            }
        }
    }

    Loader {
        id: contents
        
        sourceComponent: panel.contentItem
        anchors.fill: panel.compact ? undefined : parent
        width: panel.width
        height: panel.height
        
        onXChanged: {
            if (pullInArea.drag.active) {
                panel.showing = x > (width / 5);
            }
        }
        
        Behavior on x {
            SmoothedAnimation {
                duration: Globals.defaultAnimationTime
            }
        }

        Binding {
            when: !pullInArea.drag.active
            target: contents
            property: "x"
            value: (panel.compact && panel.showing) ? panel.width : 0
        }
    }
    
    MouseArea {
        id: pullInArea
        
        enabled: panel.compact
        width: panel.width + 10
        height: panel.height
        propagateComposedEvents: true
        x: (panel.compact && panel.showing) ? panel.width : 0
        drag {
            target: contents
            axis: Drag.XAxis
            minimumX: 0
            maximumX: panel.width
        }
    }
    
}
