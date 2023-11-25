import QtQuick 2.0
import OpenTodoList 1.0 as OTL
import "../Components" as Components
import "../Fonts" as Fonts
import "../Utils" as Utils
import "../Controls" as C

Components.ItemAction {
    property OTL.Library library: null
    required property Utils.ItemUtils itemUtils

    symbol: Fonts.Icons.mdiContentCut
    text: qsTr("Move")
    onTriggered: itemUtils.moveTodo(item, library)
}
