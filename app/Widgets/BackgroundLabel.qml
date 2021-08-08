import QtQuick 2.10

import "../Controls" as C

C.Label {
    id: label

    function linkHandler(link) {}

    anchors.fill: parent
    font.pointSize: 20
    opacity: 0.7
    verticalAlignment: Text.AlignVCenter
    horizontalAlignment: Text.AlignHCenter
    textFormat: Text.RichText
}
