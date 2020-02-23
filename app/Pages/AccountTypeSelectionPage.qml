import QtQuick 2.4
import QtQuick.Controls 2.5
import OpenTodoList 1.0 as OTL

AccountTypeSelectionPageForm {
    id: page

    signal closePage()
    signal openPage(var component, var properties)

    property Page anchorPage: null

    buttonBox.onRejected: closePage()
    buttonBox.onAccepted: {
        var webDavTypeMap = {
            "WebDAV": OTL.Account.WebDAV,
            "OwnCloud": OTL.Account.OwnCloud,
            "NextCloud": OTL.Account.NextCloud
        };

        switch (selectedAccountType) {
        case "WebDAV":
        case "NextCloud":
        case "OwnCloud":
            var type = webDavTypeMap[selectedAccountType];
            if (type !== undefined) {
                page.openPage(
                            Qt.resolvedUrl("./NewWebDAVAccountPage.qml"),
                            {
                                "type": type,
                                "anchorPage": page.anchorPage
                            });
            } else {
                console.error("Cannot handle WebDAV server type", selectedAccountType);
            }
            break;
        default:
            console.error("Unhandled account type ", selectedAccountType,
                          " in AccountTypeSelectionPageForm");
            break;
        }
    }
    Component.onCompleted: {
        var button = buttonBox.standardButton(DialogButtonBox.Ok);
        button.enabled = Qt.binding(function() {
            return selectedAccountType !== "";
        });
    }
}
