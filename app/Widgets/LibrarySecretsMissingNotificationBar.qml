import QtQuick 2.9
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12

import OpenTodoList 1.0 as OTL

import "../Components"


Pane {
    id: secretsMissingBar

    property OTL.Library library
    property bool shown: library.synchronizerSecret !== "" &&
                         OTL.Application.secretsKeys.indexOf(
                             library.synchronizerSecret) < 0

    signal openPage(var component, var properties)
    signal ignore()

    anchors {
        left: parent.left
        right: parent.right
    }
    Material.background: Material.Orange
    y: shown ? parent.height - height : parent.height

    RowLayout {
        anchors {
            left: parent.left
            right: parent.right
        }

        Label {
            text: qsTr("The secrets for synchronizing the library %1 are missing.").arg(
                      secretsMissingBar.library.name)
            Layout.fillWidth: true
        }
        Button {
            text: qsTr("Ignore")
            onClicked: secretsMissingBar.library.secretsMissing = false
        }
        Button {
            text: qsTr("Enter Secrets")
            onClicked: {
                var sync = library.createSynchronizer();
                if (sync !== null) {
                    var url = Qt.resolvedUrl(
                                "../Pages/" + sync.type + "SettingsPage.qml");
                    secretsMissingBar.openPage(url, {"synchronizer": sync});
                }
            }
        }
    }
}

