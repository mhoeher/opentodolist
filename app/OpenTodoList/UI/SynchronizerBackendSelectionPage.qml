import QtQuick 2.6
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
            image: "sync_backends/nextcloud.svg"
            synchronizer: "WebDAVSynchronizer"
            serverType: "NextCloud"
        }
        ListElement {
            name: qsTr("ownCloud")
            title: ""
            image: "sync_backends/owncloud.svg"
            synchronizer: "WebDAVSynchronizer"
            serverType: "OwnCloud"
        }
        ListElement {
            name: qsTr("WebDAV")
            title: qsTr("WebDAV")
            image: ""
            synchronizer: "WebDAVSynchronizer"
            serverType: "Generic"
        }
        ListElement {
            name: qsTr("Create local library")
            title: qsTr("Create local library")
            image: ""
            synchronizer: ""
            serverType: ""
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
        anchors.fill: parent

        GridView {
            id: grid
            anchors.fill: parent
            model: backends
            delegate: gridViewDelegate
            cellHeight: 200 + Globals.defaultMargin
            cellWidth: 400 + Globals.defaultMargin
        }
    }
}
