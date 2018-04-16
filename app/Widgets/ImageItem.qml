import QtQuick 2.0
import QtQuick.Layouts 1.1

import OpenTodoList 1.0 as OTL

import "../Components"
import "../Utils"


Item {
    id: item

    property OTL.Library library: null
    property OTL.Image libraryItem: OTL.Image {}

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
        onClicked: item.clicked()
        onReleased: item.released(mouse)
    }
}

//import QtQuick 2.0

//import OpenTodoList 1.0
//import OpenTodoList.UI 1.0

//MouseArea {
//    id: item

//    property Library library: null
//    property ImageTopLevelItem libraryItem: ImageTopLevelItem {}

//    hoverEnabled: !Globals.touchEnabled
//    acceptedButtons: Qt.LeftButton | Qt.RightButton

//    DropShadow {
//        sourceItem: background
//        hovered: item.containsMouse
//    }

//    Rectangle {
//        id: background

//        anchors {
//            fill: parent
//            margins: Globals.defaultMargin / 2
//        }

//        color: Colors.lightItemColor(libraryItem.color)
//        border {
//            width: 1
//            color: Colors.mid
//        }

//        Image {
//            id: image
//            asynchronous: true
//            source: libraryItem.imageUrl
//            anchors {
//                fill: parent
//                margins: Globals.defaultMargin
//            }

//            fillMode: Image.PreserveAspectFit
//        }
//        DueDateOverlay { dueDate: libraryItem.dueTo }
//    }
//}

