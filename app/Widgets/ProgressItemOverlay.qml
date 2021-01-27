import QtQuick 2.0

Rectangle {
    property var item: ({})

    QtObject {
        id: d

        property bool hasPercentageDone: item && item.percentageDone !== undefined
        property int percentageDone: hasPercentageDone ? item.percentageDone : 0
        property bool hasDone: item && item.done !== undefined
        property bool done: hasDone ? item.done : false
        property int effectivePercentageDone: done ? 100.0 : percentageDone
    }

    visible: d.effectivePercentageDone > 0
    width: parent.width * d.effectivePercentageDone / 100.0
    color: "black"
    opacity: 0.1
}
