import QtQuick 2.10

import "../Components"
import "../Fonts"
import "../Utils"

RoundButton {
    id: newItemButton

    signal newItem()

    anchors {
        right: parent.right
        bottom: parent.bottom
        margins: 10
    }
    symbol: Icons.faPlus
    backgroundColor: Colors.positiveColor
    onClicked: newItemButton.newItem()
}
