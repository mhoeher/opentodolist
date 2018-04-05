import QtQuick 2.0
import QtQuick.Layouts 1.1

import OpenTodoList 1.0 as OTL

import "../Components"
import "../Utils"

Item {
    id: item

    property OTL.Library library: null
    property OTL.Note libraryItem: OTL.Note {}

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

            Label {
                id: titleLabel
                text: Markdown.format(item.libraryItem.title)
                wrapMode: Text.NoWrap
                elide: Text.ElideRight
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

            Label {
                anchors.fill: parent
                text: Markdown.format(item.libraryItem.notes)
                elide: Text.ElideRight
            }
        }
    }

    MouseArea {
        anchors.fill: parent
        acceptedButtons: Qt.LeftButton | Qt.RightButton
        onClicked: item.clicked()
        onReleased: item.released(mouse)
    }
}
