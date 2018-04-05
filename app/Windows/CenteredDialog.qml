import QtQuick 2.10

import "../Components"

Dialog {
    id: dialog

    x: (parent.width - width) / 2
    y: (parent.height - height) / 2
    parent: ApplicationWindow.overlay
}
