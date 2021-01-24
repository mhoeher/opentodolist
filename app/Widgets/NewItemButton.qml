import QtQuick 2.10
import QtQuick.Controls 2.5

import "../Components" as Components
import "../Fonts"
import "../Utils"

Components.RoundButton {
    id: newItemButton

    signal newItem()

    anchors {
        right: parent.right
        bottom: parent.bottom
        margins: AppSettings.mediumSpace
    }
    symbol: Icons.faPlus
    backgroundColor: Colors.positiveColor
    onClicked: newItemButton.newItem()
}
