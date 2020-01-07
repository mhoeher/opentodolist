import QtQuick 2.9
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.12

import OpenTodoList 1.0 as OTL

import "../Components"

Page {
    id: page

    signal openPage(var component, var properties)

    title: qsTr("About...")

    ScrollView {
        id: scrollView

        anchors.fill: parent

        padding: 10

        Column {
            width: page.width - 2 * scrollView.padding
            spacing: 10

            Label {
                text: qsTr("OpenTodoList")
                font.bold: true
                width: parent.width
                padding: 5
            }

            Label {
                text: qsTr("A todo and task managing application.")
                font.italic: true
                width: parent.width
                padding: 5
            }

            Label {
                text: qsTr("(c) RPdev 2013-2020, version %1").arg(
                          applicationVersion)
                width: parent.width
                padding: 5
            }

            Label {
                text: "<a href='%1'>%1</a>".arg(
                          "https://opentodolist.rpdev.net/")
                width: parent.width
                onLinkActivated: Qt.openUrlExternally(link)
                padding: 5
            }

            Label {
                function linkHandler(link) {}

                text: qsTr("OpenTodoList is released under the terms of the " +
                           "<a href='app-license'>GNU General Public License" +
                           "</a> version 3 or (at your " +
                           "choice) any later version.")
                width: parent.width
                onLinkActivated: {
                    switch (link) {
                    case "app-license":
                        Qt.openUrlExternally(
                                    "https://gitlab.com/rpdev/opentodolist/" +
                                    "raw/master/COPYING");
                        break;
                    }
                }
                padding: 5
            }

            Button {
                anchors.right: parent.right
                text: qsTr("Report an Issue")
                onClicked: Qt.openUrlExternally(
                               "https://gitlab.com/rpdev/opentodolist/" +
                               "issues/new")
            }

            Item {
                width: 1
                height: 10
            }

            Label {
                font.bold: true
                text: qsTr("Third Party Libraries and Resources")
            }

            Repeater {
                model: OTL.Application.find3rdPartyInfos()
                delegate: itemDelegate
            }
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
            width: parent.width

            contentItem: GridLayout {
                id: column
                width: parent.width
                columnSpacing: 5
                rowSpacing: 5
                columns: 2

                Label {
                    id: itemTitle
                    text: item.name
                    font.bold: true
                    Layout.columnSpan: 2
                }

                Label {
                    text: qsTr("Author:")
                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                }

                Label {
                    text: qsTr("<a href='%2'>%1</a>").arg(
                              item.author).arg(item.website)
                    Layout.fillWidth: true
                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                    onLinkActivated: Qt.openUrlExternally(link)
                }

                Label {
                    text: qsTr("License:")
                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                }

                Label {
                    text: qsTr("<a href='%1'>%2</a>").arg(
                              item.licenseUrl).arg(item.licenseType)
                    Layout.fillWidth: true
                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                    onLinkActivated: Qt.openUrlExternally(link)
                }

                Label {
                    text: qsTr("Download:")
                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                }

                Label {
                    text: qsTr("<a href='%1'>%1</a>").arg(
                              item.downloadUrl)
                    Layout.fillWidth: true
                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                    onLinkActivated: Qt.openUrlExternally(link)
                }
            }
        }
    }
}
