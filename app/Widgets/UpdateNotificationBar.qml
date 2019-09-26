import QtQuick 2.10
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12

import "../Components"

import OpenTodoList 1.0 as OTL

Pane {
    id: updateNotificationBar

    property bool shown: false
    property string version
    property url downloadUrl

    anchors {
        left: parent.left
        right: parent.right
    }
    Material.background: Material.Green

    y: shown ? parent.height - height : parent.height

    OTL.UpdateService {
        id: updateService

        onUpdateAvailable: {
            updateNotificationBar.version = version;
            updateNotificationBar.downloadUrl = url;
            updateNotificationBar.shown = true;
        }
    }

    RowLayout {
        id: updateNotificationBarLayout
        anchors {
            verticalCenter: parent.verticalCenter
            left: parent.left
            right: parent.right
        }

        Label {
            text: qsTr("An update to OpenTodoList %1 is available.").arg(
                      updateNotificationBar.version)
            Layout.fillWidth: true
            wrapMode: Text.WordWrap
        }
        Button {
            text: qsTr("Ignore")
            onClicked: updateNotificationBar.shown = false
        }
        Button {
            text: qsTr("Download")
            onClicked: {
                Qt.openUrlExternally(updateNotificationBar.downloadUrl);
                updateNotificationBar.shown = false;
            }
        }
    }
}

