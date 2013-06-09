import QtQuick 2.0

Item {
    id: button

    property alias source: image.source
    property alias imageWidth: image.width
    property alias imageHeight: image.height

    signal clicked

    width: childrenRect.width
    height: childrenRect.height
    opacity: mouseArea.containsMouse ? 1.0 : 0.5

    Image {
        id: image
    }

    MouseArea {
        id: mouseArea

        anchors.fill: parent
        hoverEnabled: true

        onClicked: button.clicked()
    }

}
