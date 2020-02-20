import QtQuick 2.4
import QtQuick.Controls 2.5

import OpenTodoList 1.0 as OTL

NewWebDAVAccountPageForm {
    id: page

    signal closePage()
    signal returnToPage(Page page)
    signal openPage(var component, var properties)

    property Page anchorPage: null

    buttons.onRejected: closePage()
    scrollView.enabled: !dav.validating
    busyIndicator.visible: dav.validating
    errorLabel.visible: d.validated && !dav.valid
    accountNameEdit.placeholderText: {
        if (usernameEdit.text !== "" && serverAddressEdit.text !== "") {
            return usernameEdit.text + "@" + serverAddressEdit.text.replace(
                        /https?:\/\//i, "");
        } else {
            return qsTr("Account Name")
        }
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
        onClicked: {
            var url = serverAddressEdit.text;
            if (!/https?:\/\//i.exec(url)) {
                url = "https://" + url;
            }
            dav.serverType = type;
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
}
