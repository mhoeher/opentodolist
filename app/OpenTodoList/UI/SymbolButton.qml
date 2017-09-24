import QtQuick 2.0
import QtQuick.Controls 2.2

import OpenTodoList.UI 1.0

Button {
    id: button
    
    property alias symbol: button.text
    
    contentItem: Text {
        text: control.text
        font: Fonts.symbols.family
        font.pixelSize: Globals.minButtonHeight * 0.9
    }
}
