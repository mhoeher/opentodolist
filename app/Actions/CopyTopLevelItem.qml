import QtQuick 2.0
import OpenTodoList 1.0 as OTL
import "../Components" as Components
import "../Fonts" as Fonts
import "../Utils" as Utils

Components.ItemAction {
    property OTL.Library library: null

    symbol: Fonts.Icons.mdiContentCopy
    text: qsTr("Copy")
    onTriggered: Utils.ItemUtils.copyTopLevelItem(item)
}
