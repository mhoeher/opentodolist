import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3

import OpenTodoList 1.0
import OpenTodoList.UI 1.0


Rectangle {
    id: secretsMissingBar

    property Library library
    property bool shown: library.secretsMissing

    anchors {
        left: parent.left
        right: parent.right
    }
    height: secretsMissingBarLayout.height + Globals.defaultMargin
    color: Colors.warnBackground
    y: shown ? parent.height - height : parent.height

    RowLayout {
        id: secretsMissingBarLayout
        anchors {
            verticalCenter: parent.verticalCenter
            left: parent.left
            right: parent.right
            margins: Globals.defaultMargin
        }

        Label {
            text: qsTr("The secrets for synchronizing the library %1 are missing.").arg(
                      library.name)
            Layout.fillWidth: true
            wrapMode: Text.WrapAnywhere
        }
        Button {
            text: qsTr("Ignore")
            onClicked: page.library.secretsMissing = false
        }
        Button {
            text: qsTr("Enter Secrets")
            onClicked: {
                var sync = library.createSynchronizer();
                if (sync !== null) {
                    var url = Qt.resolvedUrl(sync.type + "SettingsPage.qml");
                    page.openPage(url, {"synchronizer": sync});
                }
            }
        }
    }
}

