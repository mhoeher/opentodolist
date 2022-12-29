import QtQuick 2.9
import "../Controls" as C
import "." as Widgets
import OpenTodoList 1.0 as OTL
import "../Fonts" as Fonts

C.ToolButton {
    id: moveButton

    readonly property alias dragTile: dragTile
    readonly property alias dragging: dragTile.dragging

    required property OTL.LibraryItem item
    required property var model
    required property var listViewItem

    symbol: Fonts.Icons.mdiDragHandle
    font.family: Fonts.Fonts.icons

    visible: {
        if (dragTile.dragging) {
            return true
        }
        if (listViewItem.allowReordering) {
            switch (Qt.platform.os) {
            case "ios":
            case "android":
                return true
            default:
                return !!listViewItem.hovered
            }
        } else {
            return false
        }
    }

    ItemDragTile {
        id: dragTile
        anchors.fill: parent
        item: moveButton.item
        model: moveButton.model
    }
}
