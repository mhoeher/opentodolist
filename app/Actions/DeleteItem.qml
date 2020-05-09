import QtQuick 2.0
import "../Components" as Components
import "../Fonts" as Fonts
import "../Utils" as Utils

Components.ItemAction {
    symbol: Fonts.Icons.faTrashAlt
    text: qsTr("Delete")
    onTriggered: Utils.ItemUtils.deleteItem(item)
}

