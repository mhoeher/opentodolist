import QtQuick 2.9
import QtQuick.Controls 2.2

Label {
    id: root

    property alias symbol: root.text

    anchors.fill: parent
    font.pixelSize: Math.min(width, height) * 0.8
    font.family: Fonts.symbols.name
    opacity: 0.1
    verticalAlignment: Text.AlignVCenter
    horizontalAlignment: Text.AlignHCenter
}
