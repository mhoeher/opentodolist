import QtQuick 2.10

import OpenTodoList 1.0 as OTL

import "../Utils"

MouseArea {
    id: mouseArea

    property var model

    drag.axis: Drag.YAxis
    drag.filterChildren: true
    onReleased: {
        if (drag.target !== undefined) {
            dragTile.Drag.drop();
            dragTile.x = 0;
            dragTile.y = 0;
            drag.target = undefined;
        }
    }
    onPressAndHold: {
        drag.target = dragTile;
        mouse.accepted = false;
    }

    Rectangle {
        id: itemBackground
        visible: mouseArea.drag.target === dragTile
        anchors.fill: parent
        color: Colors.color(Colors.teal)
        opacity: 0.2
        z: 9
    }

    Rectangle {
        id: dragTile

        property OTL.Item item: object

        color: Colors.color(Colors.green)
        border.color: Qt.darker(Colors.color(Colors.green))
        opacity: 0.5
        width: mouseArea.width
        height: mouseArea.height
        z: 10
        visible: mouseArea.drag.active
        Drag.hotSpot.x: width / 2
        Drag.hotSpot.y: height / 2
        Drag.active: mouseArea.drag.active
        Drag.keys: ["" + mouseArea.model]
    }

    Rectangle {
        width: parent.width
        height: 3
        color: Colors.color(Colors.teal)
        visible: upperDropArea.containsDrag
        z: 10
        y: -1
    }

    Rectangle {
        width: parent.width
        y: parent.height - 1
        height: 3
        color: Colors.color(Colors.teal)
        visible: lowerDropArea.containsDrag
        z: 10
    }

    DropArea {
        id: upperDropArea
        anchors {
            left: parent.left
            right: parent.right
            top: parent.top
        }
        height: parent.height / 2
        keys: ["" + mouseArea.model]
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
                var prevWeight = mouseArea.model.data(
                            mouseArea.model.index(index - 1, 0),
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
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }
        height: parent.height / 2
        keys: ["" + mouseArea.model]
        onDropped: {
            var item = drag.source.item;
            if (item === object) {
                return;
            }
            var thisWeight = object.weight;
            if (index === mouseArea.model.count - 1) {
                var diff = Math.abs(thisWeight) * (Math.random() * 0.1 + 0.01);
                if (diff == 0) {
                    diff = Math.random() + 0.1;
                }
                item.weight = thisWeight + diff;
            } else {
                var nextWeight = mouseArea.model.data(
                            mouseArea.model.index(index + 1, 0),
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
