import QtQuick 2.9
import QtQuick.Controls 2.2

import OpenTodoList 1.0
import OpenTodoList.UI 1.0

Page {
    id: page

    signal backendSelected(var synchronizer)
    signal cancelled()

    footer: DialogButtonBox {
        Button {
            text: qsTr("Cancel")
            onClicked: page.cancelled()
            DialogButtonBox.buttonRole: DialogButtonBox.RejectRole
        }
        Button {
            text: qsTr("OK")
            enabled: d.selectedSynchronizer >= 0
            DialogButtonBox.buttonRole: DialogButtonBox.AcceptRole
            onClicked: {
                var backend = backends.get(d.selectedSynchronizer);
                var synchronizer = backend.synchronizer;
                var serverType = backend.serverType;
                page.backendSelected({
                                         "synchronizer": synchronizer,
                                         "serverType": serverType
                                     });
            }
        }
    }

    QtObject {
        id: d

        property int selectedSynchronizer: -1
    }

    ListModel {
        id: backends
        ListElement {
            name: qsTr("NextCloud")
            title: ""
            image: "sync_backends/nextcloud.png"
            synchronizer: "WebDAVSynchronizer"
            serverType: "NextCloud"
        }
        ListElement {
            name: qsTr("ownCloud")
            title: ""
            image: "sync_backends/owncloud.png"
            synchronizer: "WebDAVSynchronizer"
            serverType: "OwnCloud"
        }
        ListElement {
            name: qsTr("WebDAV")
            title: qsTr("WebDAV")
            image: "sync_backends/cloud.png"
            synchronizer: "WebDAVSynchronizer"
            serverType: "Generic"
        }
        ListElement {
            name: qsTr("Local Library")
            title: qsTr("Local Library")
            image: "sync_backends/disk.png"
            synchronizer: ""
            serverType: ""
        }
    }

    Component {
        id: gridViewDelegate

        Item {
            width: GridView.view.cellWidth
            height: GridView.view.cellHeight
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
                anchors {
                    left: parent.left
                    right: parent.right
                    top: parent.top
                    bottom: label.top
                    margins: Globals.defaultMargin
                }

                source: {
                    if (image !== "") {
                        return Qt.resolvedUrl(image);
                    } else {
                        return "";
                    }
                }
                fillMode: Image.PreserveAspectFit
            }

            Text {
                id: label
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
        id: scrollView

        anchors.fill: parent

        GridView {
            id: grid

            property int columns: Math.max(width / (Globals.defaultMargin * 20), 1)

            x: Globals.defaultMargin
            y: Globals.defaultMargin
            width: scrollView.width - 2 * Globals.defaultMargin
            model: backends
            delegate: gridViewDelegate
            cellHeight: cellWidth * 0.66
            cellWidth: width / columns
        }
    }
}
