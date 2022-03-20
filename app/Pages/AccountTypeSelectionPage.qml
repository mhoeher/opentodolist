import QtQuick 2.4

import OpenTodoList 1.0 as OTL

import "../Components" as Components
import "../Fonts" as Fonts
import "../Utils" as Utils
import "../Controls" as C

C.Page {
    id: page

    property alias buttonBox: buttons
    property C.Page anchorPage: null

    signal accepted
    signal canceled
    signal closePage
    signal openPage(var component, var properties)

    title: qsTr("Select Account Type")

    Component.onCompleted: {
        var button = buttonBox.standardButton(C.DialogButtonBox.Ok)
        button.enabled = Qt.binding(function () {
            return d.selectedAccountType > -1
        })
    }

    footer: C.DialogButtonBox {
        id: buttons
        standardButtons: C.DialogButtonBox.Ok | C.DialogButtonBox.Cancel
        onRejected: closePage()
        onAccepted: d.openNextPage()
    }

    QtObject {
        id: d

        property int selectedAccountType: -1

        function openNextPage() {
            let accountTypeName = OTL.Application.accountTypeToString(
                    d.selectedAccountType)
            page.openPage(Qt.resolvedUrl(
                              "./New" + accountTypeName + "AccountPage.qml"), {
                              "anchorPage": page.anchorPage
                          })
        }
    }

    ListView {
        anchors.fill: parent

        header: Components.Heading {
            text: qsTr("Account Type")
            width: parent.width
            padding: Utils.AppSettings.mediumSpace
        }
        model: [{
                "type": OTL.Account.NextCloud,
                "name": qsTr("NextCloud")
            }, {
                "type": OTL.Account.OwnCloud,
                "name": qsTr("ownCloud")
            }, {
                "type": OTL.Account.WebDAV,
                "name": qsTr("WebDAV")
            }]
        delegate: C.ItemDelegate {
            width: parent.width
            text: modelData.name
            down: d.selectedAccountType === modelData.type
            onClicked: {
                d.selectedAccountType = modelData.type
                d.openNextPage()
            }
        }
    }
}
