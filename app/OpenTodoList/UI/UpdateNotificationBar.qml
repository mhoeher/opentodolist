import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3

import OpenTodoList 1.0
import OpenTodoList.UI 1.0


Rectangle {
    id: updateNotificationBar

    property bool shown: false
    property string version
    property url downloadUrl

    anchors {
        left: parent.left
        right: parent.right
    }
    height: updateNotificationBarLayout.height + Globals.defaultMargin
    color: Colors.infoBackground
    y: shown ? parent.height - height : parent.height

    UpdateService {
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
            margins: Globals.defaultMargin
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

