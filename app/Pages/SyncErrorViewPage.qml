import QtQuick 2.9
import QtQuick.Layouts 1.3

import OpenTodoList 1.0

import "../Components"
import "../Fonts"

Page {
    id: page

    property alias errors: view.model

    ListView {
        id: view

        anchors.fill: parent
        ScrollBar.vertical: ScrollBar {}

        delegate: RowLayout {
            width: view.width
            spacing: Globals.defaultMargin
            ToolButton {
                symbol: Icons.faWarning
            }
            Label {
                Layout.fillWidth: true
                wrapMode: Text.WordWrap
                text: modelData
            }
        }
    }
}
