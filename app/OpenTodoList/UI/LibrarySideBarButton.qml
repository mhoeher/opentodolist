import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3

import OpenTodoList.UI 1.0

MouseArea {
    id: root

    property alias symbol: sym.text
    property alias text: label.text
    property bool active: false
    property int indent: 0
    property bool bold: false

    height: Math.max(layout.height, Globals.minButtonHeight)
    anchors {
        left: parent.left
        right: parent.right
    }

    Rectangle {
        anchors.fill: parent
        color: Colors.highlight
        visible: root.active
        opacity: visible ? 1.0 : 0.0

        Behavior on opacity { NumberAnimation { duration: 500 } }
    }

    RowLayout {
        id: layout

        anchors {
            left: parent.left
            right: parent.right
            rightMargin: Globals.defaultMargin
            leftMargin: Globals.defaultMargin * (1 + root.indent)
            verticalCenter: parent.verticalCenter
        }

        Label {
            id: sym
            font.family: Fonts.symbols.name
            visible: text !== ""
            color: buttonContainer.isActive ? Colors.highlightedText : Colors.darkText

            Behavior on color { NumberAnimation { duration: 500 } }
        }

        Label {
            id: label

            Layout.fillWidth: true
            wrapMode: Text.WordWrap
            font.bold: root.bold
            color: sym.color
        }
    }
}

