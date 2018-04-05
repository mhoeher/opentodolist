import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Controls.Material 2.3

import "../Fonts"
import "../Utils"

RoundButton {
    id: button

    property alias symbol: button.text
    property var backgroundColor: undefined

    font.family: Fonts.solidIcons

    Material.background: backgroundColor
}
