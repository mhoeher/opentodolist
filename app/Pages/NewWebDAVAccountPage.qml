import QtQuick 2.4
import QtQuick.Controls 2.5

import OpenTodoList 1.0 as OTL

NewWebDAVAccountPageForm {
    signal closePage()
    signal openPage(var component, var properties)

    buttons.onRejected: closePage()
    scrollView.enabled: !dav.validating
    busyIndicator.visible: dav.validating

    Component.onCompleted: {
        d.okButton = buttons.standardButton(DialogButtonBox.Ok);
        errorLabel.visible = false;
    }

    QtObject {
        id: d

        property AbstractButton okButton: null
    }

    Connections {
        target: d.okButton
        onClicked: {
            var url = serverAddressEdit.text;
            if (!url.startsWith("http://") && !url.startsWith("https://")) {
                url = "https://" + url;
            }
            dav.serverType = type;
            dav.url = url;
            dav.username = usernameEdit.text;
            dav.password = passwordEdit.text;
            dav.disableCertificateCheck = disableCertificateChecksEdit.checked;
            errorLabel.visible = false;
            dav.validate();
        }
    }

    OTL.WebDAVSynchronizer {
        id: dav

        onValidatingChanged: {
            if (!validating) {
                errorLabel.visible = !dav.valid;
                console.debug(dav.valid);
            }
        }
    }
}
