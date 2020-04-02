import QtQuick 2.0

Rectangle {
    property var item: ({})

    visible: item && item.percentageDone !== undefined &&
             item.percentageDone > 0
    width: parent.width * (
               item && item.percentageDone !== undefined ? item.percentageDone : 0
               ) / 100.0
    color: "black"
    opacity: 0.1
}
