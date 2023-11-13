import QtQuick 2.0
import "../Components" as Components
import "../Fonts" as Fonts
import "../Utils" as Utils
import "../Controls" as C

Components.ItemAction {
    required property Utils.ItemUtils itemUtils

    symbol: Fonts.Icons.mdiDriveFileRenameOutline
    text: qsTr("Rename")
    onTriggered: itemUtils.renameItem(item)
}
