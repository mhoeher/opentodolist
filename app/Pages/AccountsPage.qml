import QtQuick 2.4

import Utils 1.0 as Utils
import OpenTodoList 1.0 as OTL

AccountsPageForm {
    id: page

    signal openPage(var component, var properties)

    roundButton.onClicked: {
        page.openPage(Qt.resolvedUrl("./AccountTypeSelectionPage.qml"), {
                      anchorPage: page
                      });
    }
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
}
