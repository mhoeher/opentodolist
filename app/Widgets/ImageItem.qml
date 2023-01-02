import QtQuick 2.0
import QtQuick.Controls.Material 2.12
import QtQuick.Layouts 1.1

import OpenTodoList 1.0 as OTL

import "../Components" as Components
import "../Utils"

MouseArea {
    id: item

    property OTL.Library library: null
    property OTL.ImageItem libraryItem: OTL.ImageItem {}
    property var model
    property bool allowReordering: true

    readonly property bool hovered: containsMouse

    signal clicked(var mouse)

    hoverEnabled: true
    GridView.delayRemove: moveButton.dragging
    acceptedButtons: Qt.LeftButton | Qt.RightButton

    ItemPane {
        anchors.fill: parent
        anchors.margins: AppSettings.smallSpace
        Material.elevation: 6
        item: item.libraryItem

        Image {
            source: item.libraryItem.imageUrl
            anchors.fill: parent
            fillMode: Image.PreserveAspectFit
        }
    }

    MouseArea {
        id: mouseArea

        anchors.fill: parent
        acceptedButtons: Qt.LeftButton | Qt.RightButton
        onClicked: mouse => item.clicked(mouse)
    }

    ReorderableListViewOverlay {
        id: reorderOverlay
        anchors.fill: parent
        model: item.model
        layout: Qt.Horizontal
        item: libraryItem
        dragTile: moveButton.dragTile
    }

    ItemDragButton {
        id: moveButton

        item: item.libraryItem
        model: item.model
        listViewItem: item
        anchors {
            right: parent.right
            top: parent.top
            margins: AppSettings.smallSpace
        }
    }
}
