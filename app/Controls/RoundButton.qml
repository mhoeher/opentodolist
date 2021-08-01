import QtQuick 2.10
import QtQuick.Controls 2.3 as QQC2
import QtQuick.Controls.Material 2.3

import "../Fonts" as Fonts

QQC2.RoundButton {
    id: button

    property alias symbol: button.text
    property var backgroundColor: undefined

    font.family: Fonts.Fonts.icons
    font.pixelSize: height / 2
    width: 56
    height: width

    Material.background: backgroundColor
    //Material.foreground: "white"
}
