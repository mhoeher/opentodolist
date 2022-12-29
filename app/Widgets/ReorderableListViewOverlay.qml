import QtQuick
import QtQml
import QtQuick.Controls.Material 2.12

import OpenTodoList 1.0 as OTL

import "../Utils"
import "../Controls" as C

Item {
    id: root

    property var model
    property int layout: Qt.Vertical
    property OTL.LibraryItem item
    property ItemDragTile dragTile

    QtObject {
        id: d

        readonly property var dropKeys: {
            let result = []
            if (dragTile) {
                for (let key in dragTile.Drag.mimeData) {
                    result.push(key)
                }
            }
            return result
        }

        function applyWeight(item, weight, dragTile) {
            let copy = OTL.Application.cloneItem(item)
            applyTimer.item = item
            applyTimer.weight = weight
            dragSourceBinding.target = dragTile
        }
    }

    Connections {
        id: dragSourceBinding

        function onDraggingChanged() {
            if (!target.dragging) {
                applyTimer.restart()
            }
        }

        ignoreUnknownSignals: true
    }

    Timer {
        id: applyTimer

        property OTL.LibraryItem item
        property double weight

        interval: 100
        repeat: false
        onTriggered: {
            if (item) {
                item.weight = weight
            }
            dragSourceBinding.target = null
        }
    }

    C.Pane {
        id: itemBackground
        visible: dragTile && dragTile.item == item && dragTile.dragging
        anchors.fill: parent
        Material.background: Material.Teal
        opacity: 0.2
        z: 9
    }

    C.Pane {
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
        keys: d.dropKeys
        onDropped: {
            var item = drag.source.item
            if (item === root.item) {
                return
            }
            var thisWeight = root.item.weight
            if (index === 0) {
                var diff = Math.abs(thisWeight) * (Math.random() * 0.1 + 0.01)
                if (diff === 0) {
                    diff = Math.random() + 0.1
                }
                d.applyWeight(item, thisWeight - diff, drag.source)
            } else {
                var prevWeight = root.model.data(root.model.index(index - 1,
                                                                  0),
                                                 OTL.ItemsModel.ItemRole).weight
                diff = thisWeight - prevWeight
                if (diff === 0) {
                    diff = Math.random()
                }
                var diffFactor = (0.4 + 0.2 * Math.random())
                d.applyWeight(item, thisWeight - diff * diffFactor, drag.source)
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
        keys: d.dropKeys
        onDropped: {
            var item = drag.source.item
            if (item === root.item) {
                return
            }
            var thisWeight = root.item.weight
            if (index === root.model.count - 1) {
                var diff = Math.abs(thisWeight) * (Math.random() * 0.1 + 0.01)
                if (diff === 0) {
                    diff = Math.random() + 0.1
                }
                d.applyWeight(item, thisWeight + diff, drag.source)
            } else {
                var nextWeight = root.model.data(root.model.index(index + 1,
                                                                  0),
                                                 OTL.ItemsModel.ItemRole).weight
                diff = nextWeight - thisWeight
                if (diff === 0) {
                    diff = Math.random()
                }
                var diffFactor = (0.4 + 0.2 * Math.random())
                d.applyWeight(item, thisWeight + diff * diffFactor, drag.source)
            }
        }
    }
}
