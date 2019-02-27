import QtQuick 2.10
import QtQuick.Layouts 1.3

import OpenTodoList 1.0 as OTL

import "../Components"
import "../Fonts"

Page {
    id: page

    title: qsTr("Synchronization Log")

    property alias log: view.model

    property Menu pageMenu: Menu {
        x: page.width

        MenuItem {
            text: qsTr("Copy Log")
            onClicked: OTL.Application.copyToClipboard(JSON.stringify(page.log))
        }
    }

    ListView {
        id: view

        anchors.fill: parent
        ScrollBar.vertical: ScrollBar {}

        delegate: RowLayout {
            width: view.width
            ToolButton {
                symbol: {
                    var map = {
                        "Debug": Icons.faInfo,
                        "Warning": Icons.faExclamation,
                        "Error": Icons.faTimesCircle
                    };
                    return map[modelData.type] || Icons.faQuestionCircle;
                }
            }
            Label {
                text: modelData.time
            }
            Label {
                Layout.fillWidth: true
                text: modelData.message
            }
        }
    }
}
