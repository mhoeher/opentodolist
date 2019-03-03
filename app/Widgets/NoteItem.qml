import QtQuick 2.0
import QtQuick.Layouts 1.1

import OpenTodoList 1.0 as OTL

import "../Components"
import "../Utils"

Item {
    id: item

    property OTL.Library library: null
    property OTL.Note libraryItem: OTL.Note {}
    property var model

    signal clicked()
    signal released(var mouse)

    Pane {
        anchors.fill: parent
        anchors.margins: 5
        elevation: 6
        padding: 0

        Pane {
            id: title

            anchors {
                left: parent.left
                right: parent.right
                top: parent.top
            }
            height: titleLabel.height + padding
            backgroundColor: Colors.color(Colors.itemColor(item.libraryItem),
                                          Colors.shade100)
            clip: true

            MarkdownLabel{
                id: titleLabel
                markdown: item.libraryItem.title
                wrapMode: Text.NoWrap
                elide: Text.ElideRight
                width: parent.width
            }
        }

        Pane {
            anchors {
                left: parent.left
                right: parent.right
                top: title.bottom
                bottom: parent.bottom
            }
            backgroundColor: Colors.color(Colors.itemColor(item.libraryItem),
                                          Colors.shade50)

            MarkdownLabel {
                anchors.fill: parent
                markdown: item.libraryItem.notes
                elide: Text.ElideRight
                clip: true
            }
        }
    }

    MouseArea {
        anchors.fill: parent
        acceptedButtons: Qt.LeftButton | Qt.RightButton
        onClicked: item.clicked()
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
