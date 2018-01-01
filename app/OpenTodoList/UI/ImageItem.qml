import QtQuick 2.0

import OpenTodoList 1.0
import OpenTodoList.UI 1.0

MouseArea {
    id: item

    property Library library: null
    property ImageTopLevelItem libraryItem: ImageTopLevelItem {}

    hoverEnabled: !Globals.touchEnabled
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

        color: Colors.lightItemColor(libraryItem.color)
        border {
            width: 1
            color: Colors.mid
        }

        Image {
            id: image
            asynchronous: true
            source: libraryItem.imageUrl
            anchors {
                fill: parent
                margins: Globals.defaultMargin
            }

            fillMode: Image.PreserveAspectFit
        }
        DueDateOverlay { dueDate: libraryItem.dueTo }
    }
}

