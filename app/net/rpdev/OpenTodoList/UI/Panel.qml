import QtQuick 2.0

import net.rpdev.OpenTodoList.UI 1.0


Rectangle {
    
    property int edge: Qt.LeftEdge
   
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
        anchors.centerIn: parent
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
}
