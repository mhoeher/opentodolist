import QtQuick 2.0
import "../Components" as Components
import "../Fonts" as Fonts
import "../Utils" as Utils

Components.ItemAction {
    symbol: Fonts.Icons.faPercent
    text: qsTr("Set Progress")
    onTriggered: item.progress = 0
    enabled: item && item.progress < 0
}
