import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3

import OpenTodoList 1.0
import OpenTodoList.UI 1.0

Item {
    id: root

    property BasicItem item

    height: layout.height + 2 * Globals.defaultMargin

    QtObject {
        id: d

        property bool checkable: item && (typeof(item["done"]) === "boolean")
        property bool done: checkable && item.done
    }

    RowLayout {
        id: layout

        x: Globals.defaultMargin
        y: x
        width: parent.width - 2 * x

        Symbol {
            id: checkButton
            visible: d.checkable
            symbol: d.done ? Fonts.symbols.faCheckSquareO : Fonts.symbols.faSquareO
            onClicked: root.item.done = !root.item.done
            pixelSize: Globals.minButtonHeight
        }

        URLText {
            id: label
            plainText: root.item.displayTitle
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
