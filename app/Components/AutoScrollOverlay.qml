import QtQuick 2.5

/**
  * @brief Provides automatic scrolling during drag&drop operations
  *
  * This component can be used to automatically scroll a connected Flickable during a drag
  * and drop operation.
  */
Item {
    id: root

    property Flickable flickable: null

    QtObject {
        id: d

        property double scrollDistance: 1

        function scrollUp() {
            let flickable = root.flickable;
            if (flickable) {
                flickable.contentY = flickable.contentY - parseInt(scrollDistance);
                flickable.returnToBounds();
                increaseScrollDistance();
            }
        }

        function scrollDown() {
            let flickable = root.flickable;
            if (flickable) {
                flickable.contentY = flickable.contentY + parseInt(scrollDistance);
                flickable.returnToBounds();
                increaseScrollDistance();
            }
        }

        function resetScrollDistance() {
            scrollDistance = 1;
        }

        function increaseScrollDistance() {
            scrollDistance = Math.min(scrollDistance * 1.1, fontMetrics.height);
        }
    }

    FontMetrics {
        id: fontMetrics
    }

    DropArea {
        id: scrollUpArea

        anchors {
            left: parent.left
            right: parent.right
            top: parent.top
        }
        height: fontMetrics.height * 2
    }

    DropArea {
        id: scrollDownArea

        anchors {
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }
        height: fontMetrics.height * 2
    }

    Timer {
        id: scrollUpTimer
        interval: 50
        repeat: true
        running: scrollUpArea.containsDrag
        onTriggered: d.scrollUp()
        onRunningChanged: d.resetScrollDistance()
    }

    Timer {
        id: scrollDownTimer
        interval: 50
        repeat: true
        running: scrollDownArea.containsDrag
        onTriggered: d.scrollDown()
        onRunningChanged: d.resetScrollDistance()
    }
}
