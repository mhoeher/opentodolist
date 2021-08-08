import QtQuick 2.9

import "../Fonts"
import "../Controls" as C

C.Label {
    id: root

    property alias symbol: root.text

    anchors.fill: parent
    font.pixelSize: Math.min(width, height) * 0.8
    font.family: Fonts.icons
    opacity: 0.1
    verticalAlignment: Text.AlignVCenter
    horizontalAlignment: Text.AlignHCenter
}
