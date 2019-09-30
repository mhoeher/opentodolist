import QtQuick 2.9
import QtQuick.Controls 2.12

import OpenTodoList 1.0 as OTL

import "../Components"

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

    Pane {
        anchors.fill: parent

        ListView {
            id: grid

            anchors.fill: parent
            ScrollBar.vertical: ScrollBar {}
            model: backends
            delegate: ItemDelegate {
                width: parent.width
                icon.source: image
                text: name
                highlighted: d.selectedSynchronizer === index
                onClicked: d.selectedSynchronizer = index
            }
        }
    }
}
