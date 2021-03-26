import QtQuick 2.10
import QtQuick.Controls 2.12

import "../Components"

Dialog {
    id: dialog

    readonly property int idealDialogWidth: Math.min(
                                                Qt.application.font.pixelSize *
                                                50,
                                                parent.width - 20)

    anchors.centerIn: parent
    parent: ApplicationWindow.contentItem
    modal: true
}
