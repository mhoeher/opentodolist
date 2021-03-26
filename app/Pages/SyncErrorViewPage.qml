import QtQuick 2.9
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.3

import OpenTodoList 1.0

import "../Components"
import "../Fonts"
import "../Utils" as Utils

Page {
    id: page

    property alias errors: view.model

    ListView {
        id: view

        anchors.fill: parent
        ScrollBar.vertical: ScrollBar {}

        delegate: RowLayout {
            width: view.width
            spacing: Utils.AppSettings.smallSpace
            ToolButton {
                symbol: Icons.faExclamationTriangle
                font: Fonts.fontAwesomeSolid
            }
            Label {
                Layout.fillWidth: true
                wrapMode: Text.WordWrap
                text: modelData
            }
        }
    }
}
