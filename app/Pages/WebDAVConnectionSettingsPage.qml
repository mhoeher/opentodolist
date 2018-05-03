import QtQuick 2.0
import QtQuick.Layouts 1.3

import OpenTodoList 1.0 as OTL

import "../Components"
import "../Utils"


Page {
    id: page

    signal connectionDataAvailable(var synchronizer)
    signal cancelled()

    property var synchronizer: ({})

    footer: DialogButtonBox {
        Button {
            text: dav.valid ? qsTr("OK") : qsTr("Validate")
            onClicked: {
                if (dav.valid) {
                    var synchronizer = page.synchronizer;
                    synchronizer.url = url.text;
                    synchronizer.username = username.text;
                    synchronizer.password = password.text;
                    synchronizer.disableCertificateCheck = ignoreSslErrors.checked;
                    page.connectionDataAvailable(synchronizer);
                } else {
                    d.validated = false;
                    dav.validate();
                }
            }
            enabled: !dav.validating
            DialogButtonBox.buttonRole: DialogButtonBox.AcceptRole
        }
        Button {
            text: qsTr("Cancel")
            onClicked: page.cancelled()
            DialogButtonBox.buttonRole: DialogButtonBox.RejectRole
        }
    }

    OTL.WebDAVSynchronizer {
        id: dav

        serverType: synchronizer.serverType
        url: url.text
        username: username.text
        password: password.text
        disableCertificateCheck: ignoreSslErrors.checked

        onValidatingChanged: {
            if (!validating) {
                d.validated = true;
            }
        }
        onUrlChanged: d.validated = false
        onUsernameChanged: d.validated = false
        onPasswordChanged: d.validated = false
        onDisableCertificateCheckChanged: d.validated = false
    }

    QtObject {
        id: d

        property bool validated: false
    }

    Pane {
        anchors.fill: parent

        ScrollView {
            id: scrollView
            anchors.fill: parent

            Flickable {
                width: scrollView.width

                Column {
                    width: scrollView.width

                    Label {
                        text: qsTr("Server Address:")
                        width: parent.width
                        font.bold: true
                    }
                    TextField {
                        id: url
                        placeholderText: qsTr("https://...")
                        width: parent.width
                        inputMethodHints: Qt.ImhUrlCharactersOnly | Qt.ImhNoPredictiveText
                    }

                    Label {
                        text: qsTr("User name:")
                        font.bold: true
                        width: parent.width
                    }

                    TextField {
                        id: username
                        width: parent.width
                        inputMethodHints: Qt.ImhSensitiveData | Qt.ImhNoPredictiveText
                    }

                    Label {
                        text: qsTr("Password:")
                        width: parent.width
                        font.bold: true
                    }

                    TextField {
                        id: password
                        echoMode: TextInput.Password
                        width: parent.width
                        inputMethodHints: Qt.ImhSensitiveData | Qt.ImhNoPredictiveText
                    }

                    CheckBox {
                        id: ignoreSslErrors
                        text: qsTr("Ignore SSL Errors")
                        checked: false
                    }

                    Label {
                        id: validationError
                        text: qsTr("Failed to connect to the server. Please " +
                                   "check your user name, password and the " +
                                   "server address and retry.")
                        color: Colors.color(Colors.red)
                        width: parent.width
                        visible: d.validated && !dav.valid
                    }

                    BusyIndicator {
                        visible: dav.validating
                        running: visible
                        anchors.horizontalCenter: parent.horizontalCenter
                    }
                }
            }
        }
    }
}
