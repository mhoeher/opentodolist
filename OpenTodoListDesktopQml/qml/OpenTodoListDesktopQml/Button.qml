import QtQuick 2.0

Rectangle {
    id: button
    height: parent.height
    width: label.width + 20
    color: activePalette.button
    radius: 4
    border.width: 1
    border.color: Qt.darker( activePalette.buttonText )
    
    property alias label: label.text
    property alias pressed: mouseArea.pressed
    property alias containsMouse: mouseArea.containsMouse
    
    signal clicked
    
    Text {
        id: label
        anchors.centerIn: button
        text: "Button"
        color: activePalette.buttonText
    }
    
    MouseArea {
        anchors.fill: button
        id: mouseArea
        hoverEnabled: true
        
        onClicked: button.clicked()
    }
    
    states: [
        State {
            name: "down"
            when: pressed || containsMouse
            PropertyChanges {
                target: button
                color: Qt.darker( activePalette.button )
            }
        }
    ]
    
    transitions: [
        Transition {
            from: ""
            to: "down"
            reversible: true
            ColorAnimation {
                duration: 200
                easing.type: Easing.InOutQuad
            }
        }
    ]
    
}