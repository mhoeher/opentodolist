import QtQuick 2.7
import QtQuick.Layouts 1.3

import OpenTodoList 1.0 as OTL

import "../Components"

Page {
    id: page

    property OTL.WebDAVSynchronizer synchronizer: null

    signal closePage()

    footer: DialogButtonBox {
        standardButtons: DialogButtonBox.Ok | DialogButtonBox.Cancel
        onAccepted: {
            page.synchronizer.username = username.text;
            page.synchronizer.password = password.text;
            page.synchronizer.save();
            OTL.Application.saveSynchronizerSecrets(page.synchronizer);
            page.closePage();
        }
        onRejected: page.closePage()
    }

    Pane {
        anchors.fill: parent

        ScrollView {
            id: scrollView

            anchors.fill: parent

            Column {
                width: scrollView.width

                Label {
                    text: qsTr("User name")
                    font.bold: true
                    width: parent.width
                }

                TextField {
                    id: username
                    width: parent.width
                    text: page.synchronizer.username
                    inputMethodHints: Qt.ImhSensitiveData | Qt.ImhNoPredictiveText
                }

                Label {
                    text: qsTr("Password")
                    font.bold: true
                }

                TextField {
                    id: password
                    width: parent.width
                    text: page.synchronizer.password
                    echoMode: TextInput.Password
                    inputMethodHints: Qt.ImhSensitiveData | Qt.ImhNoPredictiveText
                }
            }
        }
    }

}
