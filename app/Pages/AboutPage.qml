import QtQuick 2.9
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.12

import OpenTodoList 1.0 as OTL

import "../Components"

Page {
    id: page

    signal openPage(var component, var properties)

    title: qsTr("About...")

    Component {
        id: thirdPartyPage

        AboutThirdPartyPage {}
    }

    ScrollView {
        id: scrollView

        anchors.fill: parent

        Column {
            width: page.width - 2 * padding

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
                text: qsTr("(c) RPdev 2013-2020, version %1").arg(applicationVersion)
                width: parent.width
                padding: 5
            }

            Label {
                text: "<a href='%1'>%1</a>".arg("https://opentodolist.rpdev.net/")
                width: parent.width
                onLinkActivated: Qt.openUrlExternally(link)
                padding: 5
            }

            Label {
                function linkHandler(link) {}

                text: qsTr("OpenTodoList is released under the terms of the GNU General Public " +
                           "License version 3 or (at your choice) any later version. You can find a " +
                           "copy of the license below. Additionally, several libraries and resources " +
                           "are used. For detailed information about their license terms, please " +
                           "refer to the <a href='3rdparty'>3rd Party Software</a> page.")
                width: parent.width
                onLinkActivated: {
                    if (link == "3rdparty") {
                        page.openPage(thirdPartyPage, {});
                    }
                }
                padding: 5
            }

            Label {
                text: OTL.Application.readFile(":/res/COPYING")
                width: parent.width
                padding: 5
            }
        }
    }
}
