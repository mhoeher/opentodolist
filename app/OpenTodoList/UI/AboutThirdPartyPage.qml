import QtQuick 2.9
import QtQuick.Controls 2.2

import OpenTodoList 1.0 as OTL
import OpenTodoList.UI 1.0

Page {
    id: page

    Label {
        id: title
        font {
            pixelSize: Globals.fontPixelSize * 2
            bold: true
        }
        text: qsTr("Used Libraries and Resources")
        wrapMode: Text.WordWrap
        anchors {
            left: parent.left
            right: parent.right
            top: parent.top
            margins: Globals.defaultMargin
        }
    }

    ScrollView {
        clip: true
        anchors {
            left: parent.left
            right: parent.right
            top: title.bottom
            bottom: parent.bottom
            topMargin: Globals.defaultMargin
        }

        ListView {
            id: listView
            model: OTL.App.find3rdPartyInfos()
            delegate: itemDelegate
        }
    }

    Component {
        id: itemDelegate

        Item {
            property var item: listView.model[index]

            anchors {
                left: parent.left
                right: parent.right
                margins: Globals.defaultMargin
            }

            height: column.height + Globals.defaultMargin * 2

            Column {
                id: column
                spacing: Globals.defaultMargin / 2
                width: parent.width
                padding: Globals.defaultMargin

                Label {
                    id: itemTitle
                    text: item.name
                    font.bold: true
                    wrapMode: Text.WordWrap
                }

                Label {
                    id: authorLabel
                    text: qsTr("Author: <a href='%2'>%1</a>").arg(item.author).arg(item.website)
                    wrapMode: Text.WordWrap
                    onLinkActivated: Qt.openUrlExternally(link)
                }

                Label {
                    id: licenseLabel
                    text: qsTr("License: <a href='%1'>%2</a>").arg(item.licenseUrl).arg(item.licenseType)
                    wrapMode: Text.WordWrap
                    onLinkActivated: Qt.openUrlExternally(link)
                }

                Label {
                    id: downloadLabel
                    text: qsTr("Download: <a href='%1'>%1</a>").arg(item.downloadUrl)
                    wrapMode: Text.WordWrap
                    onLinkActivated: Qt.openUrlExternally(link)
                }
            }
        }
    }
}
