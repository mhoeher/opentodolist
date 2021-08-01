import QtQuick 2.4

import OpenTodoList 1.0 as OTL

import "../Components" as Components
import "../Fonts" as Fonts
import "../Utils" as Utils
import "../Controls" as C

C.Page {
    id: page

    property string selectedAccountType: ""
    property alias buttonBox: buttons
    property C.Page anchorPage: null

    signal accepted
    signal canceled
    signal closePage()
    signal openPage(var component, var properties)

    title: qsTr("Select Account Type")

    Component.onCompleted: {
        var button = buttonBox.standardButton(DialogButtonBox.Ok);
        button.enabled = Qt.binding(function() {
            return selectedAccountType !== "";
        });
    }

    footer: C.DialogButtonBox {
        id: buttons
        standardButtons: C.DialogButtonBox.Ok | C.DialogButtonBox.Cancel
        onRejected: closePage()
        onAccepted: d.openNextPage()
    }

    QtObject {
        id: d

        function openNextPage() {
            var webDavTypeMap = {
                "WebDAV": OTL.Account.WebDAV,
                "OwnCloud": OTL.Account.OwnCloud
            };

            switch (selectedAccountType) {
            case "NextCloud":
                page.openPage(
                            Qt.resolvedUrl("./NewNextCloudAccountPage.qml"),
                            {
                                "anchorPage": page.anchorPage
                            });
                break;
            case "WebDAV":
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
    }

    ListView {
        anchors.fill: parent

        header: Components.Heading {
            text: qsTr("Account Type")
            width: parent.width
            padding: Utils.AppSettings.mediumSpace
        }
        model: accountsModel
        delegate: C.ItemDelegate {
            width: parent.width
            text: name
            down: page.selectedAccountType === type
            onClicked: {
                page.selectedAccountType = type;
                d.openNextPage();
            }
        }
    }

    ListModel {
        id: accountsModel

        ListElement {
            type: "NextCloud"
            name: qsTr("NextCloud")
        }

        ListElement {
            type: "OwnCloud"
            name: qsTr("ownCloud")
        }

        ListElement {
            type: "WebDAV"
            name: qsTr("WebDAV")
        }
    }
}
