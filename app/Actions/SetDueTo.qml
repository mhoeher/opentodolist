import QtQuick 2.0
import "../Components" as Components
import "../Fonts" as Fonts
import "../Utils" as Utils

Components.ItemAction {
    symbol: Fonts.Icons.mdiEvent
    text: qsTr("Select Due Date")
    onTriggered: Utils.ItemUtils.selectDueToDate(item)
    enabled: item && item.dueTo !== undefined
}
