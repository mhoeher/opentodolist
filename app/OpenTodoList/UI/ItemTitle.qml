import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3

import OpenTodoList 1.0
import OpenTodoList.UI 1.0

Item {
    id: root

    property BasicItem item

    height: layout.height + 2 * Globals.defaultMargin

    RowLayout {
        id: layout

        x: Globals.defaultMargin
        y: x
        width: parent.width - 2 * x

        URLText {
            id: label
            plainText: root.item.title
            Layout.fillWidth: true
            font {
                bold: true
                pixelSize: Globals.fontPixelSize * 2
            }
            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
        }

        Symbol {
            id: editButton
            symbol: Fonts.symbols.faPencilSquareO
            onClicked: dialog.renameItem(root.item)
            pixelSize: Globals.minButtonHeight

            RenameItemDialog {
                id: dialog
            }
        }
    }
}
