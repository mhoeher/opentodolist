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
        color: Colors.lightItemColor(libraryItem.color)

        Rectangle {
            id: noteTitle

            anchors {
                left: parent.left
                right: parent.right
                top: parent.top
                margins: 1
            }

            height: Globals.fontPixelSize * 2
            color: Colors.itemColor(libraryItem.color)

            URLText {
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
                plainText: libraryItem.title
                textFormat: Text.RichText
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
            wrapMode: Text.WordWrap
            clip: true
        }

        DueDateOverlay { dueDate: libraryItem.dueTo }
    }
}
