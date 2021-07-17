import QtQuick 2.10

import "../Controls" as C
import "../Fonts" as Fonts
import "../Utils" as Utils

C.RoundButton {
    id: newItemButton

    signal newItem()

    anchors {
        right: parent.right
        bottom: parent.bottom
        margins: Utils.AppSettings.mediumSpace
    }
    symbol: Fonts.Icons.mdiAdd
    backgroundColor: Utils.Colors.positiveColor
    onClicked: newItemButton.newItem()
}
