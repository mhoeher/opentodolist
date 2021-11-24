import QtQuick 2.10

import "../Components"
import "../Controls" as C

C.Dialog {
    id: dialog

    readonly property int idealDialogWidth: Math.min(
                                                Qt.application.font.pixelSize * 80,
                                                parent.width - Qt.application.font.pixelSize * 5)

    readonly property int idealDialogHeight: Math.min(
                                                Qt.application.font.pixelSize * 60,
                                                parent.height - Qt.application.font.pixelSize * 5)

    anchors.centerIn: parent
    parent: C.ApplicationWindow.contentItem
    modal: true
}
