import QtQuick 2.0

Rectangle {
    property var item: ({})

    visible: false && item.percentageDone !== undefined &&
             item.percentageDone > 0
    width: 0 // parent.width * item.percentageDone / 100.0
    color: "black"
    opacity: 0.1
}
