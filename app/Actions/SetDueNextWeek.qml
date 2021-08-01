import QtQuick 2.0
import "../Components" as Components
import "../Fonts" as Fonts
import "../Utils" as Utils

Components.ItemAction {
    symbol: Fonts.Icons.mdiCalendarViewWeek
    text: qsTr("Set Due This Week")
    onTriggered: item.dueTo = Utils.DateUtils.endOfThisWeek()
    enabled: item && item.dueTo !== undefined
}
