import QtQuick 2.9
import QtQuick.Layouts 1.3

import OpenTodoList 1.0 as OTL

import "../Components"


Pane {
    id: secretsMissingBar

    property OTL.Library library
    property bool shown: library.secretsMissing

    signal openPage(var component, var properties)
    signal ignore()

    anchors {
        left: parent.left
        right: parent.right
    }
    backgroundColor: warn
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
                    var url = Qt.resolvedUrl(sync.type + "SettingsPage.qml");
                    secretsMissingBar.openPage(url, {"synchronizer": sync});
                }
            }
        }
    }
}

