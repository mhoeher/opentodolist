import QtQuick 2.0
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

import net.rpdev.OpenTodoList.UI 1.0

Button {
    id: button
    
    property alias symbol: button.text
    
    style: ButtonStyle {
        id: style
        
        label: Label {
            font {
                family: Fonts.symbols.family
                pixelSize: Globals.minButtonHeight * 0.9
            }
            text: style.control.text
        }
    }    
}
