import QtQuick 2.4
import QtQuick.Controls.Material 2.0
import QtQuick.Layouts 1.0

import "../Components" as Components
import "../Utils" as Utils
import "../Controls" as C
import "../Windows" 1.0 as Windows

import OpenTodoList 1.0 as OTL

C.Page {
    id: page

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

    signal closePage

    function deleteItem() {
        deleteAccountDialog.deleteAccount(account)
    }

        title: qsTr("Edit Account")
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
                                                    page.account.type)
        }

        Binding {
            target: d.okButton
            property: "enabled"
            value: accountNameEdit.displayText !== ""
        }

        Connections {
            target: d.account

            function onLoginFinished(success) {
                if (success) {
                    page.account.username = d.account.username
                    page.account.password = d.account.password
                    page.account.baseUrl = d.account.baseUrl
                    page.account.name = accountNameEdit.text
                    page.account.disableCertificateChecks = d.account.disableCertificateChecks
                    page.account.backendSpecificData = d.account.backendSpecificData

                    OTL.Application.saveAccount(page.account)
                    OTL.Application.saveAccountSecrets(page.account)
                    page.closePage()
                } else {
                    errorLabel.visible = true
                }
            }
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
                d.account.login()
            }
        }

        Windows.DeleteAccountDialog {
            id: deleteAccountDialog

            onAccepted: page.closePage()
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
                    text: qsTr("Edit Account")
                    Layout.columnSpan: 2
                    Layout.fillWidth: true
                }

                C.Label {
                    text: qsTr("Name:")
                }

                C.TextField {
                    id: accountNameEdit

                    text: account.name
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
                    text: account.baseUrl
                }

                C.Label {
                    text: qsTr("User:")
                }

                C.TextField {
                    id: usernameEdit

                    placeholderText: qsTr("User Name")
                    inputMethodHints: Qt.ImhNoAutoUppercase | Qt.ImhNoPredictiveText
                    Layout.fillWidth: true
                    text: account.username
                }

                C.Label {
                    text: qsTr("Password:")
                }

                C.PasswordField {
                    id: passwordEdit

                    text: account.password
                    placeholderText: qsTr("Password")
                    Layout.fillWidth: true
                }

                Components.Empty {}

                C.CheckBox {
                    id: disableCertificateChecksEdit

                    checked: account.disableCertificateChecks
                    text: qsTr("Disable Certificate Checks")
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

            anchors.centerIn: parent
            visible: d.account.loggingIn
        }
    }
