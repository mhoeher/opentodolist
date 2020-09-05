import QtQuick 2.4
import QtQuick.Controls 2.5

import OpenTodoList 1.0 as OTL

ProblemsPageForm {
    id: page

    signal openPage(var component, var properties)

    listView.model: OTL.Application.problemManager.problems
    listView.delegate: ItemDelegate {
        id: control

        width: parent.width
        text: {
            switch (modelData.type) {
            case "AccountSecretsMissing":
                // The secrets for an account are missing:
                let account = modelData.contextObject;
                let msg = qsTr("Missing secrets for account");
                msg += " <strong>" + account.name + "</strong>";
                return msg;
            default:
                console.warn("Unknown problem type", modelData.type);
                return "Unknown Problem";
            }
        }
        contentItem: Label {
            rightPadding: control.spacing
            text: control.text
            font: control.font
            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
            verticalAlignment: Text.AlignVCenter
        }
        onClicked: {
            switch (modelData.type) {
            case "AccountSecretsMissing":
                switch (modelData.contextObject.type) {
                case OTL.Account.NextCloud:
                    page.openPage(Qt.resolvedUrl("./EditNextCloudAccountPage.qml"), {
                                      account: modelData.contextObject
                                  });
                    break;
                case OTL.Account.OwnCloud:
                case OTL.Account.WebDAV:
                    page.openPage(Qt.resolvedUrl("./EditWebDAVAccountPage.qml"), {
                                      account: modelData.contextObject
                                  });
                    break;
                }
                break;

            default:
                console.warn("Unknown problem type", modelData.type);
            }
        }
    }
}
