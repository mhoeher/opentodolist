import QtQuick 2.10

import "../Components"

Label {
    id: label

    function linkHandler(link) {}

    anchors.fill: parent
    font.pointSize: 20
    opacity: 0.7
    verticalAlignment: Text.AlignVCenter
    horizontalAlignment: Text.AlignHCenter
}
