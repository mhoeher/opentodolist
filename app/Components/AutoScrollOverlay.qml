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

        function scrollUp() {
            let flickable = root.flickable;
            if (flickable) {
                flickable.contentY = flickable.contentY - fontMetrics.height;
                flickable.returnToBounds();
            }
        }

        function scrollDown() {
            let flickable = root.flickable;
            if (flickable) {
                flickable.contentY = flickable.contentY + fontMetrics.height;
                flickable.returnToBounds();
            }
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
        height: fontMetrics.height
    }

    DropArea {
        id: scrollDownArea

        anchors {
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }
        height: fontMetrics.height
    }

    Timer {
        id: scrollUpTimer
        interval: 10
        repeat: true
        running: scrollUpArea.containsDrag
        onTriggered: d.scrollUp()
    }

    Timer {
        id: scrollDownTimer
        interval: 10
        repeat: true
        running: scrollDownArea.containsDrag
        onTriggered: d.scrollDown()
    }
}
