import QtQuick 2.10

import OpenTodoList 1.0 as OTL

import "../Utils"

MouseArea {
    id: dragTile

    property OTL.LibraryItem item: null
    property var model

    readonly property string mimeType: "opentodolist/" + model
    readonly property bool dragging: drag.active

    drag.target: content

    Item {
        id: content

        readonly property OTL.LibraryItem item: dragTile.item
        readonly property alias dragging: dragTile.dragging

        anchors {
            horizontalCenter: parent.horizontalCenter
            verticalCenter: parent.verticalCenter
        }

        visible: dragTile.drag.active
        width: parent.width
        height: parent.height
        z: Drag.active ? 0 : 11

        Drag.hotSpot.x: width / 2
        Drag.hotSpot.y: height / 2
        Drag.dragType: Drag.Automatic
        Drag.active: dragTile.drag.active
        Drag.mimeData: {
            var result = {}
            result["opentodolist/" + dragTile.model] = ""
            return result
        }

        states: State {
            when: dragTile.dragging

            ParentChange {
                target: content
                parent: dragTile.parent
            }

            AnchorChanges {
                target: content
                anchors {
                    horizontalCenter: undefined
                    verticalCenter: undefined
                }
            }
        }
    }
}
