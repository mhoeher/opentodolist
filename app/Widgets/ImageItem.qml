import QtQuick 2.0
import QtQuick.Layouts 1.1

import OpenTodoList 1.0 as OTL

import "../Components"
import "../Utils"


Item {
    id: item

    property OTL.Library library: null
    property OTL.Image libraryItem: OTL.Image {}
    property var model

    signal clicked()
    signal released(var mouse)

    Pane {
        anchors.fill: parent
        anchors.margins: 5
        elevation: 6
        backgroundColor: Colors.color(Colors.itemColor(item.libraryItem),
                                      Colors.shade50)

        Image {
            source: item.libraryItem.imageUrl
            anchors.fill: parent
            fillMode: Image.PreserveAspectFit
        }
    }

    MouseArea {
        anchors.fill: parent
        acceptedButtons: Qt.LeftButton | Qt.RightButton
        onClicked: {
            if (mouse.button === Qt.LeftButton) {
                item.clicked()
            }
        }
        onPressAndHold: reorderOverlay.startDrag()
        onReleased: item.released(mouse)
    }

    ReorderableListViewOverlay {
        id: reorderOverlay
        anchors.fill: parent
        model: item.model
        layout: Qt.Horizontal
    }
}
