import QtQuick 2.0

import net.rpdev.OpenTodoList.UI 1.0

ButtonContainer {
    
    property alias symbol: label.text
    property alias label: label
    
    width: height
    
    Text {
        id: label
        font.family: Fonts.symbols.name
        color: "black"
        anchors.centerIn: parent
        text: Fonts.symbols.faPlus
    }
}
