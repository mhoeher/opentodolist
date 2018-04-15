import QtQuick 2.10
import QtQuick.Controls 2.3

import "../Utils"

Label {
    wrapMode: Text.WrapAtWordBoundaryOrAnywhere

    function linkHandler(link) {
        Qt.openUrlExternally(link);
    }

    onLinkActivated: linkHandler(link)
    textFormat: Text.StyledText
    linkColor: Colors.color(Colors.linkColor)

    MouseArea {
        anchors.fill: parent
        acceptedButtons: Qt.NoButton
        cursorShape: parent.hoveredLink !== "" ? Qt.PointingHandCursor :
                                                 Qt.ArrowCursor
    }
}
