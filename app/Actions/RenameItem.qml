import QtQuick 2.0
import "../Components" as Components
import "../Fonts" as Fonts
import "../Utils" as Utils

Components.ItemAction {
    symbol: Fonts.Icons.mdiDriveFileRenameOutline
    text: qsTr("Rename")
    onTriggered: Utils.ItemUtils.renameItem(item)
}

