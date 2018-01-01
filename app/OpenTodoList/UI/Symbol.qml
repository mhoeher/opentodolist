import QtQuick 2.0
import QtQuick.Controls 2.2


import OpenTodoList.UI 1.0

ButtonContainer {
    id: root
    
    property alias symbol: label.text
    property alias label: label
    property Menu menu: null
    property bool checked: false
    property alias color: label.color
    property alias pixelSize: label.font.pixelSize
    
    width: height
    
    onClicked: {
        if (menu) {
            menu.open();
        }
    }
    
    Label {
        id: label
        font {
            family: Fonts.symbols.family
            pixelSize: Globals.minButtonHeight * 0.7
        }
        anchors.centerIn: parent
        text: Fonts.symbols.faPlus
        enabled: parent.enabled
    }

    states: State {
        name: "checked"
        when: root.checked

        PropertyChanges {
            target: label
            color: Colors.accentText
        }
    }
}
