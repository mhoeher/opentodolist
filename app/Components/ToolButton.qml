import QtQuick 2.10
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12

import "../Fonts"

ToolButton {
    id: button

    property alias symbol: button.text
    property Menu menu: null
    property var backgroundColor: undefined
    property var foregroundColor: undefined

    font.family: Fonts.solidIcons
    onClicked: {
        if (menu !== null) {
            if (menu.visible) {
                menu.close();
            } else {
                menu.open();
            }
        }
    }
    Material.background: backgroundColor
    Material.foreground: foregroundColor
}
