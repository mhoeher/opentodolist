import QtQuick 2.0
import "../Components" as Components
import "../Fonts" as Fonts
import "../Utils" as Utils

Components.ItemAction {
    symbol: Fonts.Icons.faCalendarAlt
    text: qsTr("Set Due Tomorrow")
    onTriggered: item.dueTo = Utils.DateUtils.tomorrow()
    enabled: item && item.dueTo !== undefined
}
