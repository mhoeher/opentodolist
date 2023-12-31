import QtQuick 2.10
import QtQuick.Controls 2.3 as QQC2
import QtQuick.Controls.Material 2.12

import "." as C
import "../Utils" as Utils
import "../Fonts" as Fonts

QQC2.ToolButton {
    id: button

    property alias symbol: button.text
    property C.Menu menu: null
    property var backgroundColor: undefined
    property var foregroundColor: undefined

    font.family: Fonts.Fonts.icons
    font.pointSize: Utils.AppSettings.defaultFontSize * 1.3
    onClicked: if (menu) {
                   menu.open()
               }
    Material.background: backgroundColor
    Material.foreground: foregroundColor
}
