import QtQuick 2.4
import QtQuick.Controls 2.5
import QtQuick.Controls.Material 2.0
import QtQuick.Layouts 1.0

import OpenTodoList 1.0 as OTL

import Components 1.0 as Components
import Utils 1.0 as Utils
import "../Windows" 1.0 as Windows

Page {
    id: page

    property string type: ""
    property alias buttons: buttons
    property alias serverAddressEdit: serverAddressEdit
    property alias usernameEdit: usernameEdit
    property alias accountNameEdit: accountNameEdit
    property alias errorLabel: errorLabel
    property alias disableCertificateChecksEdit: disableCertificateChecksEdit
    property alias scrollView: scrollView
    property alias busyIndicator: busyIndicator
    property alias passwordEdit: passwordEdit
    property OTL.Account account

    signal closePage()

    function deleteItem() {
        deleteAccountDialog.deleteAccount(account);
    }

    title: qsTr("Edit Account")
    footer: DialogButtonBox {
        id: buttons

        standardButtons: DialogButtonBox.Ok | DialogButtonBox.Cancel
        onRejected: closePage()
    }

    Component.onCompleted: {
        d.okButton = buttons.standardButton(DialogButtonBox.Ok);
    }

    QtObject {
        id: d

        property bool validated: false
        property var okButton: null
        property OTL.NextCloudLoginFlow loginFlow: null
    }

    ScrollView {
        id: scrollView

        anchors.fill: parent
        padding: Utils.AppSettings.mediumSpace
        enabled: !dav.validating

        GridLayout {
            width: scrollView.availableWidth
            columns: 2
            columnSpacing: 10
            rowSpacing: 10
            implicitWidth: childrenRect.width
            implicitHeight: childrenRect.height

            Components.Heading {
                text: qsTr("Edit Account")
                Layout.columnSpan: 2
                Layout.fillWidth: true
            }

            Components.Label {
                text: qsTr("Name:")
            }

            TextField {
                id: accountNameEdit

                text: page.account.name
                Layout.fillWidth: true
            }

            Components.Label {
                text: qsTr("Server Address:")
            }

            TextField {
                id: serverAddressEdit

                text: page.account.baseUrl
                placeholderText: qsTr("https://myserver.example.com")
                inputMethodHints: Qt.ImhNoAutoUppercase | Qt.ImhUrlCharactersOnly
                Layout.fillWidth: true
            }

            Components.Empty {}

            Button {
                text: qsTr("Login")
                Layout.alignment: Qt.AlignRight
                onClicked: {
                    d.loginFlow = OTL.Application.createNextCloudLoginFlow(disableCertificateChecksEdit.checked);
                    d.loginFlow.startLoginFlow(serverAddressEdit.text);
                }
                enabled: serverAddressEdit.text !== "" && (d.loginFlow === null || !d.loginFlow.flowRunning)
            }

            Components.Label {
                text: qsTr("User:")
            }

            TextField {
                id: usernameEdit

                text: page.account.username
                placeholderText: qsTr("User Name")
                inputMethodHints: Qt.ImhNoAutoUppercase | Qt.ImhNoPredictiveText
                Layout.fillWidth: true
            }

            Components.Label {
                text: qsTr("Password:")
            }

            TextField {
                id: passwordEdit

                text: page.account.password
                placeholderText: qsTr("Password")
                Layout.fillWidth: true
                echoMode: TextInput.Password
                inputMethodHints: Qt.ImhSensitiveData | Qt.ImhNoPredictiveText
            }

            Item {
                width: 1
                height: 1
            }

            CheckBox {
                id: disableCertificateChecksEdit

                text: qsTr("Disable Certificate Checks")
                checked: page.account.disableCertificateChecks
            }

            Components.Label {
                id: errorLabel

                Layout.columnSpan: 2
                Layout.fillWidth: true
                text: qsTr("Failed to connect to the server. Please "
                           + "check your user name, password and the server address and retry.")
                Material.foreground: Material.Red
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                visible: d.validated && !dav.valid
            }
        }
    }

    BusyIndicator {
        id: busyIndicator

        anchors.centerIn: parent
        visible: dav.validating
    }

    Windows.DeleteAccountDialog {
        id: deleteAccountDialog

        onAccepted: page.closePage()
    }

    Binding {
        target: d.okButton
        property: "enabled"
        value: accountNameEdit.displayText !== "" && passwordEdit.text !== ""
    }

    OTL.WebDAVSynchronizer {
        id: dav

        serverType: page.account.toWebDAVServerType()

        onValidatingChanged: {
            if (!validating) {
                d.validated = true;
            }
        }

        onValidChanged: {
            if (d.validated && valid) {
                page.account.username = dav.username;
                page.account.password = dav.password;
                page.account.baseUrl = dav.url;
                page.account.name = accountNameEdit.text;
                page.account.disableCertificateChecks = dav.disableCertificateCheck;

                OTL.Application.saveAccount(page.account);
                OTL.Application.saveAccountSecrets(page.account);
                page.closePage();
            }
        }
    }

    Connections {
        target: d.loginFlow

        function onReceivedLogin(username, password, server) {
            usernameEdit.text = username;
            passwordEdit.text = password;
            serverAddressEdit.text = server;
        }

        function onReceivedLoginUrl(loginUrl) {
            Qt.openUrlExternally(loginUrl);
        }
    }

    Connections {
        target: d.okButton
        function onClicked() {
            var url = serverAddressEdit.text;
            if (!/https?:\/\//i.exec(url)) {
                url = "https://" + url;
            }
            dav.url = url;
            dav.username = usernameEdit.text;
            dav.password = passwordEdit.text;
            dav.disableCertificateCheck = disableCertificateChecksEdit.checked;
            d.validated = false;
            dav.validate();
        }
    }
}
