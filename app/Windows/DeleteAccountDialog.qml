import QtQuick 2.0

import OpenTodoList 1.0 as OTL

import "../Components"
import "../Controls" as C
import "../Utils"


CenteredDialog {
    id: dialog

    function deleteAccount(account) {
        d.account = account;
        dialog.open();
    }

    standardButtons: C.Dialog.Ok | C.Dialog.Cancel
    title: qsTr("Delete Account?")
    width: idealDialogWidth

    onAccepted: {
        var account = d.account;
        OTL.Application.removeAccount(account);
    }

    QtObject {
        id: d

        property OTL.Account account
        property string accountName: account ? account.name : ""
    }

    C.Label {
        width: dialog.availableWidth
        text: qsTr("Do you really want to remove the account " +
                   "<strong>%1</strong>? This will remove all " +
                   "libraries belonging to the account from your " +
                   "device?<br/><br/>" +
                   "<i>Note: You can restore them from the server " +
                   "by adding back the account.</i>").arg(
                  d.accountName)
    }
}
