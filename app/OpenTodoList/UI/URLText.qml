import QtQuick 2.9
import QtQuick.Controls 2.2

Label {
    id: label

    property string plainText: ""

    textFormat: Text.RichText
    text: Globals.markdownToHtml(plainText)
    onLinkActivated: Qt.openUrlExternally(link)
}
