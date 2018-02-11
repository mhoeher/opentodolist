import QtQuick 2.0
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.2
import QtQuick.Controls.Material 2.2

import OpenTodoList 1.0
import OpenTodoList.UI 1.0


Rectangle {
    id: syncIndicatorBar

    property Library library
    
    y: parent.height
    color: Colors.midlight
    border {
        width: 1
        color: Colors.mid
    }
    height: childrenRect.height
    width: parent.width
    state: library && library.synchronizing ? "visible" : ""
    
    states: State {
        name: "visible"
        
        PropertyChanges {
            target: syncIndicatorBar
            y: syncIndicatorBar.parent.height - syncIndicatorBar.height
        }
    }
    
    transitions: [
        Transition {
            from: ""
            to: "visible"
            
            SmoothedAnimation {
                properties: "y"
            }
        },
        Transition {
            from: "visible"
            to: ""
            
            SmoothedAnimation {
                properties: "y"
            }
        }
    ]
    
    RowLayout {
        height: childrenRect.height
        anchors {
            left: parent.left
            right: parent.right
            margins: Globals.defaultMargin
        }
        
        Label {
            Layout.fillWidth: true
            text: qsTr("Synchronizing library...")
        }
        
        Label {
            font.family: Fonts.symbols.name
            text: Fonts.symbols.faSpinner
            
            NumberAnimation on rotation {
                from: 0
                to: 360
                duration: 2000
                loops: Animation.Infinite
            }
        }
    }
}
