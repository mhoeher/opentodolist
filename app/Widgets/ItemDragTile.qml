import QtQuick 2.10
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12

import OpenTodoList 1.0 as OTL

import "../Utils"

Item {
    id: dragTile
    
    property OTL.LibraryItem item: null
    property Item listItem: null
    property var model: null
    
    readonly property string mimeType: "opentodolist/" + model

    function startDrag(item, listItem, model) {
        dragTile.item = item;
        dragTile.listItem = listItem;
        dragTile.model = model;
        Qt.callLater(function() {
            dragTile.Drag.active = true;
            dragTile.Drag.startDrag();
        });
    }

    
    opacity: 0.5
    width: listItem ? listItem.width : 1
    height: listItem ? listItem.height : 1
    z: Drag.active ? 0 : 11
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
