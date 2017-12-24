import QtQuick 2.0
import QtQuick.Controls 2.2

import OpenTodoList.UI 1.0

Button {
    id: button
    
    property alias symbol: button.text
    
    contentItem: Label {
        text: control.text
        font.family: Fonts.symbols.family
        font.pixelSize: Globals.minButtonHeight * 0.9
    }
}
