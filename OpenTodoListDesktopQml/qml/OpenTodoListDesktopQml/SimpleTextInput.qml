import QtQuick 2.0

Rectangle {
    id: input
    color: Qt.lighter( activePalette.button )
    radius: 2
    height: textInput.height + 10
    clip: true
    border.width: 1
    border.color: Qt.darker( activePalette.button )
    
    property alias text: textInput.text

    MouseArea {
        anchors.fill: parent
        onClicked: { parent.textInput.focus = true; }
    }
    
    TextInput {
        id: textInput
        anchors.verticalCenter: parent.verticalCenter
        anchors.left: parent.left
        anchors.right: parent.right
        text: "Simple Text Input"
        color: activePalette.buttonText
    }
}
