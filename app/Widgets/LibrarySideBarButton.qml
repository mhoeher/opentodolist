import QtQuick 2.9
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.12

import "../Fonts"
import "../Components"

ItemDelegate {
    id: root

    property alias symbol: sym.text
    property int indent: 0
    property bool bold: false
    property alias symbolIsClickable: sym.enabled

    signal symbolClicked()

    topPadding: 0
    bottomPadding: 0

    width: parent.width
    contentItem: RowLayout {
        Item {
            height: 1
            width: root.indent * 10
        }


        ToolButton {
            id: sym
            Layout.minimumWidth: root.height / 2
            Layout.alignment: Qt.AlignHCenter
            enabled: false
            onClicked: root.symbolClicked()
        }

        Label {
            id: label

            elide: Text.ElideRight
            Layout.fillWidth: true
            text: root.text
        }
    }
}


