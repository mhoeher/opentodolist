import QtQuick 2.0
import "../Components" as Components
import "../Fonts" as Fonts
import "../Utils" as Utils

Components.ItemAction {
    symbol: Fonts.Icons.mdiCalendarToday
    text: qsTr("Reset Due To")
    onTriggered: item.dueTo = new Date("")
    enabled: item && item.dueTo !== undefined
}
