import QtQuick 2.4
import QtQuick.Controls 2.5

import Components 1.0 as Components
import Fonts 1.0 as Fonts
import Utils 1.0 as Utils
import OpenTodoList 1.0 as OTL

Page {
    id: page

    property alias accounts: listView.model
    property alias roundButton: roundButton

    signal openPage(var component, var properties)
    signal accountSelected(var account)

    title: qsTr("Accounts")

    accounts: accountList.accounts
    onAccountSelected: {
        switch (account.type) {
        case OTL.Account.NextCloud:
            page.openPage(Qt.resolvedUrl("./EditNextCloudAccountPage.qml"), {
                          account: account
                          });
            break;
        case OTL.Account.OwnCloud:
        case OTL.Account.WebDAV:
            page.openPage(Qt.resolvedUrl("./EditWebDAVAccountPage.qml"), {
                          account: account
                          });
            break;
        default:
            console.error("Unhandled account type: ", account.type);
        }
    }

    Utils.Accounts {
        id: accountList
    }

    ListView {
        id: listView

        anchors.fill: parent

        header: Components.Heading {
            text: qsTr("Accounts")
            padding: 10
        }
        delegate: ItemDelegate {
            id: itemDelegate

            text: name
            width: parent.width

            Connections {
                target: itemDelegate

                function onClicked() {
                    page.accountSelected(account);
                }
            }
        }
    }

    Components.RoundButton {
        id: roundButton
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.margins: 10
        symbol: Fonts.Icons.faPlus
        backgroundColor: Utils.Colors.positiveColor
        onClicked: {
            page.openPage(Qt.resolvedUrl("./AccountTypeSelectionPage.qml"), {
                          anchorPage: page
                          });
        }
    }
}
