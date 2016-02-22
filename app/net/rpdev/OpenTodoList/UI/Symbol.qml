import QtQuick 2.0
import QtQuick.Controls 1.4

import net.rpdev.OpenTodoList.UI 1.0

ButtonContainer {
    
    property alias symbol: label.text
    property alias label: label
    property Menu menu: null
    
    width: height
    
    onClicked: {
        if (menu) {
            menu.popup();
        }
    }
    
    Text {
        id: label
        font.family: Fonts.symbols.name
        font.bold: false
        color: "black"
        anchors.centerIn: parent
        text: Fonts.symbols.faPlus
    }
}
