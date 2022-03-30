import QtQuick 2.4
import QtQuick.Controls.Material 2.0
import QtQuick.Layouts 1.0

import OpenTodoList 1.0 as OTL

import "../Components" as Components
import "../Utils" as Utils
import "../Controls" as C

C.Page {
    id: page

    signal closePage
    signal returnToPage(C page)
    signal openPage(var component, var properties)

    property C.Page anchorPage: null

    title: qsTr("Connection Settings")
    footer: C.DialogButtonBox {
        id: buttons

        standardButtons: C.DialogButtonBox.Ok | C.DialogButtonBox.Cancel
        onRejected: closePage()
    }

    Component.onCompleted: {
        d.okButton = buttons.standardButton(C.DialogButtonBox.Ok)
    }

    QtObject {
        id: d

        property var okButton: null
        property bool manualLogin: false
        property OTL.NextCloudLoginFlow loginFlow: null

        function baseUrl(fromUrl) {
            var url = fromUrl
            if (!/https?:\/\//i.exec(url)) {
                url = "https://" + url
            }
            return url
        }

        function urlForCreatingAppSpecificPasswords(fromUrl) {
            let url = d.baseUrl(fromUrl)
            if (!url.endsWith("/")) {
                url += "/"
            }
            url += "settings/user/security"
            return url
        }
    }

    C.ScrollView {
        id: scrollView

        anchors.fill: parent
        padding: Utils.AppSettings.mediumSpace
        enabled: account.loggingIn

        GridLayout {
            width: scrollView.availableWidth
            columns: 2
            columnSpacing: 10
            rowSpacing: 10
            implicitWidth: childrenRect.width
            implicitHeight: childrenRect.height

            Components.Heading {
                text: qsTr("Connection Settings")
                Layout.columnSpan: 2
                Layout.fillWidth: true
            }

            C.Label {
                text: qsTr("Server Address:")
            }

            C.TextField {
                id: serverAddressEdit

                placeholderText: "https://nextcloud.example.com"
                inputMethodHints: Qt.ImhNoAutoUppercase | Qt.ImhUrlCharactersOnly
                Layout.fillWidth: true
            }

            Components.Empty {}

            C.Button {
                text: qsTr("Login")
                Layout.alignment: Qt.AlignRight
                onClicked: {
                    d.loginFlow = OTL.Application.createNextCloudLoginFlow(
                                disableCertificateChecksEdit.checked)
                    d.loginFlow.startLoginFlow(d.baseUrl(
                                                   serverAddressEdit.text))
                    loginInfoBox.visible = true
                }
                enabled: serverAddressEdit.text !== ""
                         && (d.loginFlow === null || !d.loginFlow.flowRunning)
            }

            C.GroupBox {
                id: loginInfoBox

                title: qsTr("Trouble Signing In?")
                Layout.columnSpan: 2
                Layout.fillWidth: true
                visible: false

                Column {
                    width: parent.width
                    spacing: Utils.AppSettings.mediumSpace

                    C.Label {
                        width: parent.width
                        text: qsTr(
                                  "We have tried to open your browser to log you in to "
                                  + "your NextCloud instance. Please log in and grant access "
                                  + "to OpenTodoList in order to proceed. Trouble accessing "
                                  + "your NextCloud in the browser? You can manually enter "
                                  + "your username and password as well.")
                    }
                    C.Button {
                        text: qsTr("Log in Manually")
                        onClicked: d.manualLogin = true
                        anchors.right: parent.right
                    }
                    C.Label {
                        width: parent.width
                        text: qsTr(
                                  "Ideally, you use app specific passwords instead of your "
                                  + "user password. In case your login is protected with "
                                  + "2 Factor Authentication (2FA) you even must use app specific "
                                  + "passwords to access your NextCloud. You can create such "
                                  + "passwords in your user settings.")
                    }

                    C.Button {
                        text: qsTr("Create App Password")
                        onClicked: buttonMenu.open()
                        anchors.right: parent.right

                        C.Menu {
                            id: buttonMenu
                            modal: true

                            C.MenuItem {
                                text: qsTr("Account Settings")
                                onTriggered: {
                                    let url = serverAddressEdit.text
                                    url = d.urlForCreatingAppSpecificPasswords(
                                                url)
                                    Qt.openUrlExternally(url)
                                    console.debug(url)
                                }
                            }
                            C.MenuItem {
                                text: qsTr("Copy Link")
                                onTriggered: {
                                    let url = serverAddressEdit.text
                                    url = d.urlForCreatingAppSpecificPasswords(
                                                url)
                                    OTL.Application.copyToClipboard(url)
                                    C.ToolTip.show(qsTr("Copied!"), 5000)
                                }
                            }
                        }
                    }
                }
            }

            C.Label {
                text: qsTr("User:")
                visible: d.manualLogin
            }

            C.TextField {
                id: usernameEdit

                placeholderText: qsTr("User Name")
                inputMethodHints: Qt.ImhNoAutoUppercase | Qt.ImhNoPredictiveText
                Layout.fillWidth: true
                visible: d.manualLogin
            }

            C.Label {
                text: qsTr("Password:")
                visible: d.manualLogin
            }

            C.PasswordField {
                id: passwordEdit

                placeholderText: qsTr("Password")
                Layout.fillWidth: true
                visible: d.manualLogin
            }

            Components.Empty {}

            C.CheckBox {
                id: disableCertificateChecksEdit

                text: qsTr("Disable Certificate Checks")
            }

            C.Label {
                text: qsTr("Name:")
            }

            C.TextField {
                id: accountNameEdit

                Layout.fillWidth: true
                placeholderText: {
                    if (usernameEdit.text !== ""
                            && serverAddressEdit.text !== "") {
                        return usernameEdit.text + "@" + serverAddressEdit.text.replace(
                                    /https?:\/\//i, "")
                    } else {
                        return qsTr("Account Name")
                    }
                }
            }

            C.Label {
                id: errorLabel

                Layout.columnSpan: 2
                Layout.fillWidth: true
                text: qsTr("Failed to connect to the server. Please "
                           + "check your user name, password and the server address and retry.")
                Material.foreground: Material.Red
            }
        }
    }

    C.BusyIndicator {
        id: busyIndicator

        anchors.centerIn: parent
        visible: account.loggingIn
    }

    OTL.NextCloudAccount {
        id: account

        name: {
            if (accountNameEdit.text !== "") {
                return accountNameEdit.text
            } else {
                return accountNameEdit.placeholderText
            }
        }

        onLoginFinished: {
            if (success) {
                OTL.Application.saveAccount(account)
                OTL.Application.saveAccountSecrets(account)
                page.returnToPage(page.anchorPage)
            } else {
                errorLabel.visible = true
            }
        }
    }

    Connections {
        target: d.loginFlow

        function onReceivedLogin(username, password, server) {
            usernameEdit.text = username
            passwordEdit.text = password
            serverAddressEdit.text = server
            loginInfoBox.visible = false
        }

        function onReceivedLoginUrl(loginUrl) {
            Qt.openUrlExternally(loginUrl)
        }
    }

    Connections {
        target: d.okButton

        function onClicked() {
            console.debug("Start validation")
            account.baseUrl = d.baseUrl()
            account.baseUrl = d.baseUrl(serverAddressEdit.text)
            account.username = usernameEdit.text
            account.password = passwordEdit.text
            account.disableCertificateChecks = disableCertificateChecksEdit.checked
            errorLabel.visible = false
            account.login()
        }
    }

    Binding {
        target: d.okButton
        property: "enabled"
        value: serverAddressEdit.text !== "" && usernameEdit.text !== ""
               && passwordEdit.text !== ""
    }
}
