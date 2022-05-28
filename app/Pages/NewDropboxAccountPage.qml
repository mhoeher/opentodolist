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
    signal returnToPage(var page)
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
        property url authorizationUrl: ""
    }

    C.ScrollView {
        id: scrollView

        anchors.fill: parent
        padding: Utils.AppSettings.mediumSpace

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

            C.GroupBox {
                id: loginInfoBox

                title: qsTr("Trouble Signing In?")
                Layout.columnSpan: 2
                Layout.fillWidth: true
                visible: account.loggingIn

                Column {
                    width: parent.width
                    spacing: Utils.AppSettings.mediumSpace

                    C.Label {
                        width: parent.width
                        text: qsTr(
                                  "We have tried to open your browser to log you in to "
                                  + "your Dropbox account. Please log in and grant access "
                                  + "to OpenTodoList in order to proceed.")
                    }
                    C.Label {
                        width: parent.width
                        text: qsTr(
                                  "Didn't your browser open? You can retry opening it or copy "
                                  + "the required URL manually to your browser.")
                    }

                    C.Button {
                        text: qsTr("Authorize...")
                        onClicked: buttonMenu.popup()
                        anchors.right: parent.right

                        C.Menu {
                            id: buttonMenu
                            modal: true

                            C.MenuItem {
                                text: qsTr("Open Browser")
                                onTriggered: {
                                    Qt.openUrlExternally(d.authorizationUrl)
                                }
                            }
                            C.MenuItem {
                                text: qsTr("Copy Link")
                                onTriggered: {
                                    OTL.Application.copyToClipboard(
                                                d.authorizationUrl)
                                    C.ToolTip.show(qsTr("Copied!"), 5000)
                                }
                            }
                        }
                    }
                }
            }

            C.Label {
                text: qsTr("Name:")
            }

            C.TextField {
                id: accountNameEdit

                Layout.fillWidth: true
                placeholderText: qsTr("Dropbox")
                enabled: !account.loggingIn
            }

            C.Label {
                id: errorLabel

                Layout.columnSpan: 2
                Layout.fillWidth: true
                textFormat: Text.StyledText
                Material.foreground: Material.Red
                visible: false
            }
        }
    }

    C.BusyIndicator {
        id: busyIndicator

        anchors.centerIn: parent
        visible: account.loggingIn
    }

    OTL.DropboxAccount {
        id: account

        name: {
            if (accountNameEdit.text !== "") {
                return accountNameEdit.text
            } else {
                return accountNameEdit.placeholderText
            }
        }

        onLoginFinished: success => {
                             if (success) {
                                 OTL.Application.saveAccount(account)
                                 OTL.Application.saveAccountSecrets(account)
                                 page.returnToPage(page.anchorPage)
                             } else {
                                 errorLabel.visible = true
                             }
                         }
        onLoginError: message => errorLabel.text = message
        onLoggingInChanged: if (loggingIn) {
                                errorLabel.visible = false
                            }
        onAuthorizationUrlReceived: url => d.authorizationUrl = url
    }

    Connections {
        target: d.okButton

        function onClicked() {
            account.login()
        }
    }
}
