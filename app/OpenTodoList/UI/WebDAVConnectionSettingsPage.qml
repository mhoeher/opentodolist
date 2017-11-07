import QtQuick 2.0
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3

import OpenTodoList 1.0
import OpenTodoList.UI 1.0

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
                    dav.validate();
                }
            }
            DialogButtonBox.buttonRole: DialogButtonBox.AcceptRole
        }
        Button {
            text: qsTr("Cancel")
            onClicked: page.cancelled()
            DialogButtonBox.buttonRole: DialogButtonBox.RejectRole
        }
    }


    WebDAVSynchronizer {
        id: dav

        serverType: synchronizer.serverType
        url: url.text
        username: username.text
        password: password.text
        disableCertificateCheck: ignoreSslErrors.checked
    }

    ScrollView {
        id: scrollView
        anchors.fill: parent

        Flickable {
            width: scrollView.width

            Column {
                width: scrollView.width - Globals.defaultMargin * 2
                x: Globals.defaultMargin
                y: Globals.defaultMargin


                Item {
                    height: Globals.defaultMargin
                    width: 1
                }
                Label {
                    text: qsTr("Server Address:")
                    width: parent.width
                }
                TextField {
                    id: url
                    placeholderText: qsTr("https://...")
                    width: parent.width
                    inputMethodHints: Qt.ImhUrlCharactersOnly | Qt.ImhNoPredictiveText
                    selectByMouse: true
                }

                Item {
                    height: Globals.defaultMargin
                    width: 1
                }
                Label {
                    text: qsTr("User name:")
                    width: parent.width
                }
                TextField {
                    id: username
                    width: parent.width
                    inputMethodHints: Qt.ImhSensitiveData | Qt.ImhNoPredictiveText
                    selectByMouse: true
                }

                Item {
                    height: Globals.defaultMargin
                    width: 1
                }
                Label {
                    text: qsTr("Password:")
                    width: parent.width
                }
                TextField {
                    id: password
                    echoMode: TextInput.Password
                    width: parent.width
                    inputMethodHints: Qt.ImhSensitiveData | Qt.ImhNoPredictiveText
                    selectByMouse: true
                }

                Item {
                    height: Globals.defaultMargin
                    width: 1
                }
                CheckBox {
                    id: ignoreSslErrors
                    text: qsTr("Ignore SSL Errors")
                    checked: false
                }

                Item {
                    height: Globals.defaultMargin
                    width: 1
                }
                BusyIndicator {
                    visible: dav.validating
                    anchors.horizontalCenter: parent.horizontalCenter
                }
            }
        }
    }
}
