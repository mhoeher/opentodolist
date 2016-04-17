import QtQuick 2.0
import QtQuick.Controls 1.4

import net.rpdev.OpenTodoList.UI 1.0

ButtonContainer {
    
    property alias symbol: label.text
    property alias label: label
    property Menu menu: null
    property bool checked: false
    property alias color: label.color
    property alias pixelSize: label.font.pixelSize
    
    width: height
    
    onClicked: {
        if (menu) {
            menu.popup();
        }
    }
    
    Text {
        id: label
        font {
            family: Fonts.symbols.family
            pixelSize: Globals.minButtonHeight * 0.7
        }
        color: checked ? Colors.primary : "black"
        anchors.centerIn: parent
        text: Fonts.symbols.faPlus
    }
}
