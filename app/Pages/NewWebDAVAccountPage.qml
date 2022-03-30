import QtQuick 2.4
import QtQuick.Controls.Material 2.0
import QtQuick.Layouts 1.0

import OpenTodoList 1.0 as OTL
import "../Components" as Components
import "../Utils" as Utils
import "../Controls" as C

C.Page {
    id: page

    property int type: OTL.Account.WebDAV
    property alias buttons: buttons
    property alias serverAddressEdit: serverAddressEdit
    property alias usernameEdit: usernameEdit
    property alias accountNameEdit: accountNameEdit
    property alias errorLabel: errorLabel
    property alias disableCertificateChecksEdit: disableCertificateChecksEdit
    property alias scrollView: scrollView
    property alias busyIndicator: busyIndicator
    property alias passwordEdit: passwordEdit
    property C.Page anchorPage: null

    signal closePage
    signal returnToPage(C page)
    signal openPage(var component, var properties)

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
        property OTL.WebDAVAccount account: OTL.Application.createAccount(
                                                page.type)
    }

    Connections {
        target: d.okButton
        function onClicked() {
            var url = serverAddressEdit.text
            if (!/https?:\/\//i.exec(url)) {
                url = "https://" + url
            }

            d.account.baseUrl = url
            d.account.username = usernameEdit.text
            d.account.password = passwordEdit.text
            d.account.disableCertificateChecks = disableCertificateChecksEdit.checked
            errorLabel.visible = false
            d.login()
        }
    }

    Connections {
        target: d.account

        function onLoginFinished(success) {
            if (success) {
                if (accountNameEdit.text !== "") {
                    account.name = accountNameEdit.text
                } else {
                    account.name = accountNameEdit.placeholderText
                }
                OTL.Application.saveAccount(account)
                OTL.Application.saveAccountSecrets(account)
                page.returnToPage(page.anchorPage)
            } else {
                errorLabel.visible = true
            }
        }
    }

    C.ScrollView {
        id: scrollView

        anchors.fill: parent
        padding: Utils.AppSettings.mediumSpace
        enabled: !d.account.loggingIn

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

                placeholderText: qsTr("https://myserver.example.com")
                inputMethodHints: Qt.ImhNoAutoUppercase | Qt.ImhUrlCharactersOnly
                Layout.fillWidth: true
            }

            C.Label {
                text: qsTr("User:")
            }

            C.TextField {
                id: usernameEdit

                placeholderText: qsTr("User Name")
                inputMethodHints: Qt.ImhNoAutoUppercase | Qt.ImhNoPredictiveText
                Layout.fillWidth: true
            }

            C.Label {
                text: qsTr("Password:")
            }

            C.PasswordField {
                id: passwordEdit

                placeholderText: qsTr("Password")
                Layout.fillWidth: true
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
                visible: false
            }
        }
    }

    C.BusyIndicator {
        id: busyIndicator

        visible: d.account.loggingIn
        anchors.centerIn: parent
    }
}
