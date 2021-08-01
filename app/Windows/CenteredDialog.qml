import QtQuick 2.10

import "../Components"
import "../Controls" as C

C.Dialog {
    id: dialog

    readonly property int idealDialogWidth: Math.min(
                                                Qt.application.font.pixelSize *
                                                50,
                                                parent.width - 20)

    anchors.centerIn: parent
    parent: C.ApplicationWindow.contentItem
    modal: true
}
