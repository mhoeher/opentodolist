import QtQuick 2.10
import QtQuick.Controls 2.3

Label {
    wrapMode: Text.WrapAtWordBoundaryOrAnywhere

    function linkHandler(link) {
        Qt.openUrlExternally(link);
    }

    onLinkActivated: linkHandler(link)
    textFormat: Text.RichText

    MouseArea {
        anchors.fill: parent
        acceptedButtons: Qt.NoButton
        cursorShape: parent.hoveredLink !== "" ? Qt.PointingHandCursor :
                                                 Qt.ArrowCursor
    }
}
