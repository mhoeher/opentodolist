import QtQuick 2.0
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.2
import QtGraphicalEffects 1.0 as GE

import OpenTodoList 1.0
import OpenTodoList.UI 1.0


MouseArea {
    id: item

    property Library library: null
    property Note libraryItem: Note {}

    hoverEnabled: true
    acceptedButtons: Qt.LeftButton | Qt.RightButton

    DropShadow {
        sourceItem: background
        hovered: item.containsMouse
    }

    Rectangle {
        id: background

        anchors {
            fill: parent
            margins: Globals.defaultMargin / 2
        }
        border {
            width: 1
            color: Colors.mid
        }
        clip: true

        Image {
            id: backgroundImage
            source: "paper/paper.png"
            fillMode: Image.Tile
            anchors {
                fill: parent
                margins: background.border.width
            }
        }

        GE.ColorOverlay {
            source: backgroundImage
            anchors.fill: backgroundImage
            color: Colors.makeTransparent(Colors.itemColor(libraryItem.color), 0.5)
        }

        Rectangle {
            id: noteTitle

            anchors {
                left: parent.left
                right: parent.right
                top: parent.top
            }

            height: Globals.fontPixelSize * 2
            color: Qt.tint(Colors.midlight,
                           Colors.makeTransparent(
                               Colors.itemColor(libraryItem.color), 0.7))

            Label {
                anchors {
                    left: parent.left
                    right: parent.right
                    verticalCenter: parent.verticalCenter
                    margins: Globals.defaultMargin / 2
                    topMargin: Globals.fontPixelSize / 2
                }
                font {
                    pixelSize: Globals.fontPixelSize
                    bold: true
                }
                elide: Text.ElideRight
                text: libraryItem.displayTitle
            }
        }

        Label {
            anchors {
                left: parent.left
                right: parent.right
                top: noteTitle.bottom
                bottom: parent.bottom
                margins: Globals.defaultMargin / 2
            }

            textFormat: Text.RichText
            text: Globals.markdownToHtml(libraryItem.notes)
            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
            clip: true
        }
    }
}
