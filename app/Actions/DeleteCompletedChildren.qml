import QtQuick 2.0
import "../Components" as Components
import "../Fonts" as Fonts
import "../Utils" as Utils

Components.ItemAction {
    symbol: Fonts.Icons.mdiRemoveDone
    text: qsTr("Delete Completed Items")
    onTriggered: Utils.ItemUtils.deleteCompletedItems(item)
}
