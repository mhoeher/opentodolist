import QtQuick 2.0
import QtQuick.Layouts 1.1

import OpenTodoList 1.0 as OTL

import "../Components"
import "../Fonts"

Pane {
    id: syncIndicatorBar

    property OTL.Library library

    readonly property bool syncRunning: {
        return library &&
                OTL.Application.directoriesWithRunningSync.indexOf(
                    library.directory) >= 0;
    }
    
    y: parent.height
    backgroundColor: nonInteractive
    width: parent.width
    state: syncRunning ? "visible" : ""
    
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
        anchors {
            left: parent.left
            right: parent.right
        }
        
        Label {
            Layout.fillWidth: true
            text: qsTr("Synchronizing library...")
        }
        
        Label {
            font.family: Fonts.solidIcons
            text: Icons.faSpinner
            
            NumberAnimation on rotation {
                from: 0
                to: 360
                duration: 2000
                loops: Animation.Infinite
                running: syncIndicatorBar.state === "visible"
            }
        }
    }
}
