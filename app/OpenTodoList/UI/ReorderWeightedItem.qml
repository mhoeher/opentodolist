import QtQuick 2.9

import OpenTodoList 1.0
import OpenTodoList.UI 1.0

Item {
    id: root

    property var model

    signal clicked()

    MouseArea {
        id: mouseArea

        anchors.fill: parent
        drag.axis: Drag.YAxis
        onClicked: root.clicked()
        onReleased: {
            dragTile.Drag.drop();
            dragTile.x = 0;
            dragTile.y = 0;
            drag.target = undefined;
        }
        onPressAndHold: {
            drag.target = dragTile;
            mouse.accepted = false;
        }
    }

    Rectangle {
        id: itemBackground
        visible: mouseArea.drag.target === dragTile
        anchors.fill: parent
        color: Colors.highlight
        opacity: 0.2
        z: 9
    }

    Rectangle {
        id: dragTile

        property BasicItem item: object

        color: Colors.midlight
        border.color: Colors.mid
        opacity: 0.5
        width: root.width
        height: root.height
        visible: mouseArea.drag.active
        Drag.hotSpot.x: width / 2
        Drag.hotSpot.y: height / 2
        Drag.active: mouseArea.drag.active
        Drag.keys: ["" + root.model]
    }

    Rectangle {
        width: parent.width
        height: 3
        color: Colors.dark
        visible: upperDropArea.containsDrag
        z: 10
        y: -1
    }

    Rectangle {
        width: parent.width
        y: parent.height - 1
        height: 3
        color: Colors.dark
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
        keys: ["" + root.model]
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
                            ItemsModel.ItemRole).weight;
                var diff = thisWeight - prevWeight;
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
        keys: ["" + root.model]
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
                            ItemsModel.ItemRole).weight;
                var diff = nextWeight - thisWeight;
                if (diff === 0) {
                    diff = Math.random();
                }
                var diffFactor = (0.4 + 0.2 * Math.random());
                item.weight = thisWeight + diff * diffFactor;
            }
        }
    }
}
