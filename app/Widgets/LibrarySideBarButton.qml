import QtQuick 2.9
import QtQuick.Layouts 1.3

import "../Fonts"
import "../Components"
import "../Controls" as C
import "../Utils" as Utils

C.ItemDelegate {
    id: root

    property alias symbol: sym.text
    property int indent: 0
    property bool bold: false
    property alias symbolIsClickable: sym.enabled
    property alias rightSymbol: symRight.text
    property alias rightSymbolIsClickable: symRight.enabled
    property alias rightSymbolIsVisible: symRight.visible
    property alias symbolToolButton: sym
    property alias leftColorSwatch: leftColorSwatch

    signal symbolClicked
    signal rightSymbolClicked

    symbol: action?.symbol ?? ""

    topPadding: 0
    bottomPadding: 0
    leftPadding: 0

    width: parent.width
    contentItem: RowLayout {
        Rectangle {
            id: leftColorSwatch

            height: sym.implicitHeight
            width: Utils.AppSettings.smallSpace
            color: "transparent"
        }

        Item {
            height: 1
            width: root.indent * Utils.AppSettings.mediumSpace
        }

        C.ToolButton {
            id: sym
            Layout.minimumWidth: root.height / 2
            Layout.alignment: Qt.AlignHCenter
            enabled: false
            onClicked: root.symbolClicked()
        }

        C.Label {
            id: label

            elide: Text.ElideRight
            Layout.fillWidth: true
            text: root.text
            wrapMode: Text.NoWrap
        }

        C.ToolButton {
            id: symRight
            Layout.minimumWidth: root.height / 2
            Layout.alignment: Qt.AlignHCenter
            enabled: false
            onClicked: root.rightSymbolClicked()
            visible: false
        }
    }
}
