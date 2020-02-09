import QtQuick 2.4
import QtQuick.Controls 2.5

import OpenTodoList 1.0 as OTL

import "../Windows" 1.0 as Windows

EditWebDAVAccountPageForm {
    id: page

    property OTL.Account account

    signal closePage()

    function deleteItem() {
        deleteAccountDialog.deleteAccount(account);
    }

    usernameEdit.text: account.username
    passwordEdit.text: account.password
    accountNameEdit.text: account.name
    serverAddressEdit.text: account.baseUrl
    disableCertificateChecksEdit.checked: account.disableCertificateChecks
    errorLabel.visible: d.validated && !dav.valid
    scrollView.enabled: !dav.validating
    busyIndicator.visible: dav.validating
    buttons.onRejected: closePage()

    Component.onCompleted: {
        d.okButton = buttons.standardButton(DialogButtonBox.Ok);
    }

    QtObject {
        id: d

        property bool validated: false
        property AbstractButton okButton: null
    }

    Binding {
        target: d.okButton
        property: "enabled"
        value: accountNameEdit.displayText !== ""
    }

    OTL.WebDAVSynchronizer {
        id: dav

        serverType: page.account.toWebDAVServerType()

        onValidatingChanged: {
            if (!validating) {
                d.validated = true;
            }
        }

        onValidChanged: {
            if (d.validated && valid) {
                page.account.username = dav.username;
                page.account.password = dav.password;
                page.account.baseUrl = dav.url;
                page.account.name = accountNameEdit.text;
                page.account.disableCertificateChecks = dav.disableCertificateCheck;

                OTL.Application.saveAccount(page.account);
                OTL.Application.saveAccountSecrets(page.account);
                page.closePage();
            }
        }
    }

    Connections {
        target: d.okButton
        onClicked: {
            var url = serverAddressEdit.text;
            if (!/https?:\/\//i.exec(url)) {
                url = "https://" + url;
            }
            dav.url = url;
            dav.username = usernameEdit.text;
            dav.password = passwordEdit.text;
            dav.disableCertificateCheck = disableCertificateChecksEdit.checked;
            d.validated = false;
            dav.validate();
        }
    }

    Windows.DeleteAccountDialog {
        id: deleteAccountDialog

        onAccepted: page.closePage()
    }

}
