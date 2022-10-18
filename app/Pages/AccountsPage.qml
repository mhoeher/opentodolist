import QtQuick 2.4

import "../Components" as Components
import "../Controls" as C
import "../Fonts" as Fonts
import "../Utils" as Utils

import OpenTodoList 1.0 as OTL

C.Page {
    id: page

    property alias accounts: listView.model
    property alias roundButton: roundButton

    signal openPage(var component, var properties)
    signal accountSelected(var account)

    title: qsTr("Accounts")

    accounts: accountList.accounts
    onAccountSelected: {
        let accountTypeName = OTL.Application.accountTypeToString(account.type)
        page.openPage(Qt.resolvedUrl(
                          "./Edit" + accountTypeName + "AccountPage.qml"), {
                          "account": OTL.Application.loadAccount(
                                         account.uid) // Load a copy of the account to ensure we have latest secrets
                      })
    }

    Utils.Accounts {
        id: accountList
    }

    ListView {
        id: listView

        anchors.fill: parent

        header: Components.Heading {
            text: qsTr("Accounts")
            padding: Utils.AppSettings.mediumSpace
        }
        delegate: C.ItemDelegate {
            id: itemDelegate

            text: name
            width: listView.width

            Connections {
                target: itemDelegate

                function onClicked() {
                    page.accountSelected(account)
                }
            }
        }
    }

    C.RoundButton {
        id: roundButton
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.margins: Utils.AppSettings.mediumSpace
        symbol: Fonts.Icons.mdiAdd
        backgroundColor: Utils.Colors.positiveColor
        onClicked: {
            page.openPage(Qt.resolvedUrl("./AccountTypeSelectionPage.qml"), {
                              "anchorPage": page
                          })
        }
    }
}
