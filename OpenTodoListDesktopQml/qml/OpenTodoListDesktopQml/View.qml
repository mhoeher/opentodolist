import QtQuick 2.0

Rectangle {
    id: view
    x: parent.x + parent.width
    width: parent.width
    height: parent.height
    clip: true
    
    property alias toolButtons: toolbarContent.children
    property bool hidden: true
    default property alias content: contentPanel.children
    
    MouseArea {
        anchors.fill: parent
    }
    
    Rectangle {
        id: toolbar
        height: 40
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        color: activePalette.window
        border.color: Qt.darker( activePalette.button )
        border.width: 2
        radius: 10
        clip: true
        
        Flickable {
            anchors.fill: parent
            
            Row {
                id: toolbarContent
                x: parent.x + 5
                y: parent.y + 5
                width: parent.width - 10
                height: parent.height - 10
                spacing: 3
            }
        }
    }
    
    Rectangle {
        anchors.top: toolbar.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.topMargin: 10
        color: activePalette.window
        
        Flickable {
            id: contentPanel
            anchors.fill: parent
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