import QtQuick 2.9
import QtQuick.Controls 2.2

import OpenTodoList.UI 1.0

Label {
    anchors.fill: parent
    anchors.margins: Globals.defaultMargin
    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
    font.pixelSize: 2 * Globals.fontPixelSize
    opacity: 0.7
    verticalAlignment: Text.AlignVCenter
    horizontalAlignment: Text.AlignHCenter
}
