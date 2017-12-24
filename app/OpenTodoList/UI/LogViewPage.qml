import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3

import OpenTodoList 1.0
import OpenTodoList.UI 1.0

Page {
    id: page

    property alias log: view.model

    property Menu pageMenu: Menu {
        x: page.width

        MenuItem {
            text: qsTr("Copy Log")
            onClicked: App.copyToClipboard(JSON.stringify(page.log))
        }
    }

    ListView {
        id: view

        anchors.fill: parent
        ScrollBar.vertical: ScrollBar {}

        delegate: RowLayout {
            width: view.width
            spacing: Globals.defaultMargin
            Symbol {
                symbol: {
                    "Debug": Fonts.symbols.faInfo,
                    "Warning": Fonts.symbols.faWarning,
                    "Error": Fonts.symbols.faTimesCircle
                }[modelData.type] || Fonts.symbols.faQuestionCircleO
            }
            Label {
                text: modelData.time
            }
            Label {
                Layout.fillWidth: true
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                text: modelData.message
            }
        }
    }
}
