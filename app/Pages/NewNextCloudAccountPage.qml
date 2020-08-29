import QtQuick 2.4
import QtQuick.Controls 2.5
import QtQuick.Controls.Material 2.0
import QtQuick.Layouts 1.0

import OpenTodoList 1.0 as OTL

import Components 1.0 as Components
import Utils 1.0 as Utils

Page {
    id: page

    signal closePage()
    signal returnToPage(Page page)
    signal openPage(var component, var properties)

    property Page anchorPage: null

    title: qsTr("Connection Settings")
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

        property var okButton: null
        property bool validated: false
        property bool manualLogin: false

        function baseUrl(fromUrl) {
            var url = fromUrl;
            if (!/https?:\/\//i.exec(url)) {
                url = "https://" + url;
            }
            return url;
        }

        function urlForCreatingAppSpecificPasswords(fromUrl) {
            let url = d.baseUrl(fromUrl);
            if (!url.endsWith("/")) {
                url += "/";
            }
            url += "settings/user/security";
            return url;
        }
    }

    ScrollView {
        id: scrollView

        anchors.fill: parent
        padding: 10
        enabled: !dav.validating

        GridLayout {
            width: scrollView.availableWidth
            columns: 2
            columnSpacing: 10
            rowSpacing: 10

            Components.Heading {
                text: qsTr("Connection Settings")
                Layout.columnSpan: 2
                Layout.fillWidth: true
            }

            Components.Label {
                text: qsTr("Server Address:")
            }

            TextField {
                id: serverAddressEdit

                placeholderText: "https://nextcloud.example.com"
                inputMethodHints: Qt.ImhNoAutoUppercase | Qt.ImhUrlCharactersOnly
                Layout.fillWidth: true
            }

            Item { width: 1; height: 1 }

            Button {
                text: qsTr("Login")
                Layout.alignment: Qt.AlignRight
                onClicked: {
                    loginFlow.startLoginFlow(d.baseUrl(serverAddressEdit.text));
                    loginInfoBox.visible = true;
                }
                enabled: serverAddressEdit.text !== ""
            }

            GroupBox {
                id: loginInfoBox

                title: qsTr("Trouble Signing In?")
                Layout.columnSpan: 2
                Layout.fillWidth: true
                visible: false

                Column {
                    width: parent.width
                    spacing: 10

                    Components.Label {
                        width: parent.width
                        text: qsTr("We have tried to open your browser to log you in to " +
                                   "your NextCloud instance. Please log in and grant access " +
                                   "to OpenTodoList in order to proceed. Trouble accessing " +
                                   "your NextCloud in the browser? You can manually enter " +
                                   "your username and password as well.");
                    }
                    Button {
                        text: qsTr("Log in Manually")
                        onClicked: d.manualLogin = true;
                        anchors.right: parent.right
                    }
                    Components.Label {
                        width: parent.width
                        text: qsTr("Ideally, you use app specific passwords instead of your " +
                                   "user password. In case your login is protected with " +
                                   "2 Factor Authentication (2FA) you even must use app specific " +
                                   "passwords to access your NextCloud. You can create such " +
                                   "passwords in your user settings.")
                    }

                    Button {
                        text: qsTr("Create App Password")
                        onClicked: buttonMenu.open()
                        anchors.right: parent.right

                        Menu {
                            id: buttonMenu
                            modal: true

                            MenuItem {
                                text: qsTr("Account Settings")
                                onTriggered: {
                                    let url = serverAddressEdit.text;
                                    url = d.urlForCreatingAppSpecificPasswords(url);
                                    Qt.openUrlExternally(url);
                                    console.debug(url);
                                }
                            }
                            MenuItem {
                                text: qsTr("Copy Link")
                                onTriggered: {
                                    let url = serverAddressEdit.text;
                                    url = d.urlForCreatingAppSpecificPasswords(url);
                                    OTL.Application.copyToClipboard(url);
                                    ToolTip.show(qsTr("Copied!"), 5000);
                                }
                            }
                        }
                    }
                }
            }

            Components.Label {
                text: qsTr("User:")
                visible: d.manualLogin
            }

            TextField {
                id: usernameEdit

                placeholderText: qsTr("User Name")
                inputMethodHints: Qt.ImhNoAutoUppercase | Qt.ImhNoPredictiveText
                Layout.fillWidth: true
                visible: d.manualLogin
            }

            Components.Label {
                text: qsTr("Password:")
                visible: d.manualLogin
            }

            TextField {
                id: passwordEdit

                placeholderText: qsTr("Password")
                Layout.fillWidth: true
                echoMode: TextInput.Password
                inputMethodHints: Qt.ImhSensitiveData | Qt.ImhNoPredictiveText
                visible: d.manualLogin
            }

            Item {
                width: 1
                height: 1
            }

            CheckBox {
                id: disableCertificateChecksEdit

                text: qsTr("Disable Certificate Checks")
            }

            Components.Label {
                text: qsTr("Name:")
            }

            TextField {
                id: accountNameEdit

                Layout.fillWidth: true
                placeholderText: {
                    if (usernameEdit.text !== "" && serverAddressEdit.text !== "") {
                        return usernameEdit.text + "@" + serverAddressEdit.text.replace(
                                    /https?:\/\//i, "");
                    } else {
                        return qsTr("Account Name")
                    }
                }
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

    OTL.WebDAVSynchronizer {
        id: dav

        serverType: OTL.WebDAVSynchronizer.NextCloud

        onValidatingChanged: {
            if (!validating) {
                d.validated = true;
            }
        }
        onValidChanged: {
            if (d.validated && valid) {
                OTL.Application.saveAccount(account);
                OTL.Application.saveAccountSecrets(account);
                page.returnToPage(page.anchorPage);
            }
        }
    }

    OTL.Account {
        id: account

        type: OTL.Account.NextCloud
        baseUrl: dav.url
        username: dav.username
        password: dav.password
        name: {
            if (accountNameEdit.text !== "") {
                return accountNameEdit.text;
            } else {
                return accountNameEdit.placeholderText;
            }
        }

        disableCertificateChecks: dav.disableCertificateCheck
    }

    OTL.NextCloudLoginFlow {
        id: loginFlow

        onReceivedLoginUrl: Qt.openUrlExternally(loginUrl)
        onReceivedLogin: {
            usernameEdit.text = username;
            passwordEdit.text = password;
            serverAddressEdit.text = server;
            loginInfoBox.visible = false;
            d.validated = false;
        }
    }

    Connections {
        target: d.okButton

        function onClicked() {
            console.debug("Start validation");
            dav.url = d.baseUrl(serverAddressEdit.text);
            dav.username = usernameEdit.text;
            dav.password = passwordEdit.text;
            dav.disableCertificateCheck = disableCertificateChecksEdit.checked;
            d.validated = false;
            dav.validate();
        }
    }

    Binding {
        target: d.okButton
        property: "enabled"
        value: serverAddressEdit.text !== "" && usernameEdit.text !== "" && passwordEdit.text !== ""
    }
}

