import QtQuick 2.4
import QtQuick.Controls 2.5

AccountTypeSelectionPageForm {
    id: page

    signal closePage()
    signal openPage(var component, var properties)

    property Page anchorPage: null

    buttonBox.onRejected: closePage()
    buttonBox.onAccepted: {
        switch (selectedAccountType) {
        case "WebDAV":
        case "OwnCloud":
        case "NextCloud":
            page.openPage(
                        Qt.resolvedUrl("./NewWebDAVAccountPage.qml"),
                        {
                            "type": selectedAccountType,
                            "anchorPage": page.anchorPage
                        });
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
