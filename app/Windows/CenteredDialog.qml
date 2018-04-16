import QtQuick 2.10

import "../Components"

Dialog {
    id: dialog

    readonly property int idealDialogWidth: Math.min(
                                                Qt.application.font.pixelSize *
                                                50,
                                                parent.width - 20)

    x: (parent.width - width) / 2
    y: (parent.height - height) / 2
    parent: ApplicationWindow.contentItem
}
