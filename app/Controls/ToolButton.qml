import QtQuick 2.10
import QtQuick.Controls 2.3 as QQC2
import QtQuick.Controls.Material 2.12

import "." as C
import "../Fonts" as Fonts

QQC2.ToolButton {
    id: button

    property alias symbol: button.text
    property C.Menu menu: null
    property var backgroundColor: undefined
    property var foregroundColor: undefined

    font.family: Fonts.Fonts.icons
    onClicked: {
        if (menu) {
            if (menu.visible) {
                menu.close()
            } else {
                menu.open()
            }
        }
    }
    Material.background: backgroundColor
    Material.foreground: foregroundColor
}
