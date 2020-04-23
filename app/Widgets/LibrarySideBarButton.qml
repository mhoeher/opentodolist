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
    property alias symbolIsClickable: symMouseArea.enabled

    signal symbolClicked()

    width: parent.width
    contentItem: RowLayout {
        Item {
            height: 1
            width: root.indent * 10
        }


        Label {
            id: sym
            font {
                family: Fonts.solidIcons
                bold: root.bold
            }
            width: root.height
            height: width
            Layout.minimumWidth: root.height / 2
            Layout.alignment: Qt.AlignHCenter

            MouseArea {
                id: symMouseArea
                anchors.fill: parent
                onClicked: root.symbolClicked()
            }
        }

        Label {
            id: label

            elide: Text.ElideRight
            Layout.fillWidth: true
            text: root.text
        }
    }
}


