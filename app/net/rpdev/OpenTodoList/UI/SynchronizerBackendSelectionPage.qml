import QtQuick 2.6
import QtQuick.Controls 1.4

import net.rpdev.OpenTodoList 1.0
import net.rpdev.OpenTodoList.UI 1.0

Item {
    id: page

    signal backendSelected(Synchronizer synchronizer)
    signal cancelled()

    QtObject {
        id: d

        property int selectedSynchronizer: -1
    }

    ListModel {
        id: backends
        ListElement {
            name: qsTr("NextCloud")
            title: ""
            image: "sync_backends/nextcloud.svg"
        }
        ListElement {
            name: qsTr("ownCloud")
            title: ""
            image: "sync_backends/owncloud.svg"
        }
        ListElement {
            name: qsTr("WebDAV")
            title: qsTr("WebDAV")
            image: ""
        }
        ListElement {
            name: qsTr("Create local library")
            title: qsTr("Create local library")
            image: ""
        }
    }

    Component {
        id: gridViewDelegate

        Item {
            width: 400
            height: 200
            clip: true

            Rectangle {
                anchors.fill: parent
                visible: d.selectedSynchronizer === index
                color: Colors.primary
                opacity: 0.2
                border {
                    color: Qt.darker(Colors.primary)
                    width: 1
                }
                radius: Globals.defaultMargin
            }

            MouseArea {
                anchors.fill: parent
                onClicked: d.selectedSynchronizer = index
            }

            Image {
                anchors.centerIn: parent
                source: {
                    if (image !== "") {
                        return Qt.resolvedUrl(image);
                    } else {
                        return "";
                    }
                }
                height: 150
                fillMode: Image.PreserveAspectFit
            }

            Text {
                anchors {
                    horizontalCenter: parent.horizontalCenter
                    bottom: parent.bottom
                    bottomMargin: Globals.defaultMargin
                }
                text: title
            }
        }
    }

    ScrollView {
        anchors {
            left: parent.left
            top: parent.top
            right: parent.right
            bottom: buttons.top
        }

        GridView {
            id: grid
            anchors.fill: parent
            model: backends
            delegate: gridViewDelegate
            cellHeight: 200 + Globals.defaultMargin
            cellWidth: 400 + Globals.defaultMargin
        }
    }

    Row {
        id: buttons

        anchors {
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }
        layoutDirection: Qt.RightToLeft
        padding: Globals.defaultMargin
        spacing: Globals.defaultMargin

        Button {
            id: okButton
            text: qsTr("OK")
        }

        Button {
            id: cancelButton
            text: qsTr("Cancel")
            onClicked: page.cancelled()
        }
    }
}
