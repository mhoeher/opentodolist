import QtQuick 2.9
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.2

import OpenTodoList 1.0
import OpenTodoList.UI 1.0

Page {
    id: page

    signal openPage(var component, var properties)

    Component {
        id: thirdPartyPage

        AboutThirdPartyPage {}
    }

    ScrollView {
        id: scrollView

        anchors.fill: parent

        Column {
            width: page.width - 2 * padding
            spacing: Globals.defaultMargin
            padding: Globals.defaultMargin

            Label {
                text: qsTr("OpenTodoList")
                font {
                    pixelSize: Globals.fontPixelSize * 2
                    bold: true
                }
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                width: parent.width
            }

            Label {
                text: qsTr("A todo and task managing application.")
                font.bold: true
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                width: parent.width
            }

            Label {
                text: qsTr("(c) RPdev 2013-2018, version %1").arg(applicationVersion)
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                width: parent.width
            }

            Label {
                text: "<a href='%1'>%1</a>".arg("https://www.rpdev.net/wordpress/apps/opentodolist/")
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                width: parent.width
                onLinkActivated: Qt.openUrlExternally(link)
            }

            Label {
                text: qsTr("OpenTodoList is released under the terms of the GNU General Public " +
                           "License version 3 or (at your choice) any later version. You can find a " +
                           "copy of the license below. Additionally, several libraries and resources " +
                           "are used. For detailed information about their license terms, please " +
                           "refer to the <a href='3rdparty'>3rd Party Software</a> page.")
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                width: parent.width
                onLinkActivated: {
                    if (link == "3rdparty") {
                        page.openPage(thirdPartyPage, {});
                    }
                }
            }

            Label {
                text: App.readFile(":/res/COPYING")
                width: parent.width
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
            }
        }
    }
}
