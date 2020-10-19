import QtQuick 2.10
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12

import OpenTodoList 1.0 as OTL

import "../Utils"

Item {
    id: root

    property var model
    property int layout: Qt.Vertical
    property OTL.LibraryItem item
    property ItemDragTile dragTile

    function startDrag() {
        dragTile.startDrag(item, root, model);
    }

    Pane {
        id: itemBackground
        visible: dragTile && dragTile.item === item && dragTile.Drag.active
        anchors.fill: parent
        Material.background: Material.Teal
        opacity: 0.2
        z: 9
    }

    Pane {
        width: root.layout === Qt.Vertical ? parent.width : 3
        height: root.layout === Qt.Vertical ? 3 : parent.height
        Material.background: Material.Teal
        visible: upperDropArea.containsDrag
        z: 10
        y: -1
    }

    Rectangle {
        width: root.layout === Qt.Vertical ? parent.width : 3
        y: root.layout === Qt.Vertical ? parent.height - 1 : 0
        x: root.layout === Qt.Vertical ? 0 : parent.width - 1
        height: root.layout === Qt.Vertical ? 3 : parent.height
        Material.background: Material.Teal
        visible: lowerDropArea.containsDrag
        z: 10
    }

    DropArea {
        id: upperDropArea
        anchors {
            left: parent.left
            right: root.layout === Qt.Vertical ? parent.right : parent.horizontalCenter
            top: parent.top
            bottom: root.layout === Qt.Vertical ? parent.verticalCenter : parent.bottom
        }
        keys: dragTile? [dragTile.mimeType] : []
        onDropped: {
            console.warn(drag.source);
            console.warn(drag.source.item);
            var item = drag.source.item;
            if (item === object) {
                return;
            }
            var thisWeight = object.weight;
            if (index === 0) {
                var diff = Math.abs(thisWeight) * (Math.random() * 0.1 + 0.01);
                if (diff === 0) {
                    diff = Math.random() + 0.1;
                }
                item.weight = thisWeight - diff;
            } else {
                var prevWeight = root.model.data(
                            root.model.index(index - 1, 0),
                            OTL.LibraryItemsModel.ItemRole).weight;
                diff = thisWeight - prevWeight;
                if (diff === 0) {
                    diff = Math.random();
                }
                var diffFactor = (0.4 + 0.2 * Math.random());
                item.weight = thisWeight - diff * diffFactor;
            }
        }
    }

    DropArea {
        id: lowerDropArea
        anchors {
            left: root.layout === Qt.Vertical ? parent.left : parent.horizontalCenter
            right: parent.right
            bottom: parent.bottom
            top: root.layout === Qt.Vertical ? parent.verticalCenter : parent.top
        }
        keys: dragTile ? [dragTile.mimeType] : []
        onDropped: {
            var item = drag.source.item;
            if (item === object) {
                return;
            }
            var thisWeight = object.weight;
            if (index === root.model.count - 1) {
                var diff = Math.abs(thisWeight) * (Math.random() * 0.1 + 0.01);
                if (diff === 0) {
                    diff = Math.random() + 0.1;
                }
                item.weight = thisWeight + diff;
            } else {
                var nextWeight = root.model.data(
                            root.model.index(index + 1, 0),
                            OTL.LibraryItemsModel.ItemRole).weight;
                diff = nextWeight - thisWeight;
                if (diff === 0) {
                    diff = Math.random();
                }
                var diffFactor = (0.4 + 0.2 * Math.random());
                item.weight = thisWeight + diff * diffFactor;
            }
        }
    }
}
