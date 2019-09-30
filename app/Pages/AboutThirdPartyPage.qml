import QtQuick 2.10
import QtQuick.Controls 2.12

import OpenTodoList 1.0 as OTL

import "../Components"

Page {
    id: page

    title: qsTr("Used Libraries and Resources")

    ScrollView {
        clip: true
        anchors.fill: parent

        ListView {
            id: listView
            model: OTL.Application.find3rdPartyInfos()
            delegate: itemDelegate
        }
    }

    Component {
        id: itemDelegate

        ItemDelegate {
            property var item: modelData

            anchors {
                left: parent.left
                right: parent.right
            }

            contentItem: Column {
                id: column
                width: parent.width

                Label {
                    id: itemTitle
                    text: item.name
                    font.bold: true
                }

                Label {
                    id: authorLabel
                    text: qsTr("Author: <a href='%2'>%1</a>").arg(
                              item.author).arg(item.website)
                    onLinkActivated: Qt.openUrlExternally(link)
                }

                Label {
                    id: licenseLabel
                    text: qsTr("License: <a href='%1'>%2</a>").arg(
                              item.licenseUrl).arg(item.licenseType)
                    onLinkActivated: Qt.openUrlExternally(link)
                }

                Label {
                    id: downloadLabel
                    text: qsTr("Download: <a href='%1'>%1</a>").arg(
                              item.downloadUrl)
                    onLinkActivated: Qt.openUrlExternally(link)
                }
            }
        }
    }
}
