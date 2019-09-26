import QtQuick 2.10
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12

import "../Utils"

Label {
    wrapMode: Text.WrapAtWordBoundaryOrAnywhere

    function linkHandler(link) {
        Qt.openUrlExternally(link);
    }

    onLinkActivated: linkHandler(link)
    textFormat: Text.StyledText
    linkColor: Material.color(Colors.linkColor)

    MouseArea {
        anchors.fill: parent
        acceptedButtons: Qt.NoButton
        cursorShape: parent.hoveredLink !== "" ? Qt.PointingHandCursor :
                                                 Qt.ArrowCursor
    }
}
