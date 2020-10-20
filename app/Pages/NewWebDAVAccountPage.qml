import QtQuick 2.4
import QtQuick.Controls 2.5
import QtQuick.Controls.Material 2.0
import QtQuick.Layouts 1.0

import OpenTodoList 1.0 as OTL
import Components 1.0 as Components
import Utils 1.0 as Utils

Page {
    id: page

    property int type: -1
    property alias buttons: buttons
    property alias serverAddressEdit: serverAddressEdit
    property alias usernameEdit: usernameEdit
    property alias accountNameEdit: accountNameEdit
    property alias errorLabel: errorLabel
    property alias disableCertificateChecksEdit: disableCertificateChecksEdit
    property alias scrollView: scrollView
    property alias busyIndicator: busyIndicator
    property alias passwordEdit: passwordEdit
    property Page anchorPage: null

    signal closePage()
    signal returnToPage(Page page)
    signal openPage(var component, var properties)

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
    }

    Connections {
        target: d.okButton
        function onClicked() {
            var url = serverAddressEdit.text;
            if (!/https?:\/\//i.exec(url)) {
                url = "https://" + url;
            }
            switch (account.type) {
            case OTL.Account.NextCloud:
                dav.serverType = OTL.WebDAVSynchronizer.NextCloud;
                break;
            case OTL.Account.OwnCloud:
                dav.serverType = OTL.WebDAVSynchronizer.OwnCloud;
                break;
            case OTL.Account.WebDAV:
                dav.serverType = OTL.WebDAVSynchronizer.Generic;
                break;
            default:
                console.error("Unhandled account type", account.type);
                return;
            }
            dav.url = url;
            dav.username = usernameEdit.text;
            dav.password = passwordEdit.text;
            dav.disableCertificateCheck = disableCertificateChecksEdit.checked;
            d.validated = false;
            dav.validate();
        }
    }

    OTL.WebDAVSynchronizer {
        id: dav

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

        type: page.type
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

                placeholderText: qsTr("https://myserver.example.com")
                inputMethodHints: Qt.ImhNoAutoUppercase | Qt.ImhUrlCharactersOnly
                Layout.fillWidth: true
            }

            Components.Label {
                text: qsTr("User:")
            }

            TextField {
                id: usernameEdit

                placeholderText: qsTr("User Name")
                inputMethodHints: Qt.ImhNoAutoUppercase | Qt.ImhNoPredictiveText
                Layout.fillWidth: true
            }

            Components.Label {
                text: qsTr("Password:")
            }

            TextField {
                id: passwordEdit

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

        visible: dav.validating
        anchors.centerIn: parent
    }
}
