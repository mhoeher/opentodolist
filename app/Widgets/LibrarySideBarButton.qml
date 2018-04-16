import QtQuick 2.9
import QtQuick.Layouts 1.3

import "../Fonts"
import "../Components"

ItemDelegate {
    id: root

    property alias symbol: sym.text
    property int indent: 0
    property bool bold: false

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
        }

        Label {
            id: label

            elide: Text.ElideRight
            Layout.fillWidth: true
            text: root.text
        }
    }
}


