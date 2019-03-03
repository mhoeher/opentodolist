import QtQuick 2.10

import OpenTodoList 1.0 as OTL

import "../Utils"

Item {
    id: root

    property var model
    property int layout: Qt.Vertical

    function startDrag() {
        dragTile.Drag.active = true;
        dragTile.Drag.startDrag();
    }

    Rectangle {
        id: itemBackground
        visible: dragTile.Drag.active
        anchors.fill: parent
        color: Colors.color(Colors.teal)
        opacity: 0.2
        z: 9
    }

    Item {
        id: dragTile

        property OTL.Item item: object

        readonly property string mimeType: "opentodolist/" + root.model

        opacity: 0.5
        width: root.width
        height: root.height
        z: Drag.active ? 0 : 11
        anchors.top: Drag.active ? undefined : parent.top
        visible: dragTile.Drag.active
        Drag.hotSpot.x: width / 2
        Drag.hotSpot.y: height / 2
        Drag.dragType: Drag.Automatic
        Drag.mimeData: {
            var result = {};
            result[dragTile.mimeType] = "";
            return result;
        }
    }

    Rectangle {
        width: root.layout === Qt.Vertical ? parent.width : 3
        height: root.layout === Qt.Vertical ? 3 : parent.height
        color: Colors.color(Colors.teal)
        visible: upperDropArea.containsDrag
        z: 10
        y: -1
    }

    Rectangle {
        width: root.layout === Qt.Vertical ? parent.width : 3
        y: root.layout === Qt.Vertical ? parent.height - 1 : 0
        x: root.layout === Qt.Vertical ? 0 : parent.width - 1
        height: root.layout === Qt.Vertical ? 3 : parent.height
        color: Colors.color(Colors.teal)
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
        keys: [dragTile.mimeType]
        onDropped: {
            var item = drag.source.item;
            if (item === object) {
                return;
            }
            var thisWeight = object.weight;
            if (index === 0) {
                var diff = Math.abs(thisWeight) * (Math.random() * 0.1 + 0.01);
                if (diff == 0) {
                    diff = Math.random() + 0.1;
                }
                item.weight = thisWeight - diff;
            } else {
                var prevWeight = root.model.data(
                            root.model.index(index - 1, 0),
                            OTL.ItemsModel.ItemRole).weight;
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
        keys: [dragTile.mimeType]
        onDropped: {
            var item = drag.source.item;
            if (item === object) {
                return;
            }
            var thisWeight = object.weight;
            if (index === root.model.count - 1) {
                var diff = Math.abs(thisWeight) * (Math.random() * 0.1 + 0.01);
                if (diff == 0) {
                    diff = Math.random() + 0.1;
                }
                item.weight = thisWeight + diff;
            } else {
                var nextWeight = root.model.data(
                            root.model.index(index + 1, 0),
                            OTL.ItemsModel.ItemRole).weight;
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
