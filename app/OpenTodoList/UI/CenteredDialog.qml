import QtQuick 2.10
import QtQuick.Controls 2.3

import OpenTodoList.UI 1.0

Dialog {
    x: (parent.width - width) / 2
    y: (parent.height - height) / 2
    width: Math.min(parent.width * 0.7, 800)
    parent: ApplicationWindow.overlay
    padding: Globals.defaultPadding
    dim: true
}
