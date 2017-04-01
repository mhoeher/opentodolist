import QtQuick 2.0

import net.rpdev.OpenTodoList 1.0
import net.rpdev.OpenTodoList.UI 1.0

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
            margins: item.containsMouse ? Globals.defaultMargin / 2 : Globals.defaultMargin
        }

        color: Qt.tint("white", Colors.makeTransparent(
                           Colors.itemColor(libraryItem.color), 0.5))
        border {
            width: 1
            color: Colors.itemDelimiter
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

        Behavior on anchors.margins {
            SmoothedAnimation {
                duration: Globals.defaultAnimationTime
            }
        }
    }
}

