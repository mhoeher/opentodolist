import QtQuick 2.10
import QtQuick.Layouts 1.1

import OpenTodoList 1.0 as OTL

import "../Components" as Components
import "../Controls" as C
import "../Utils" as Utils

C.Page {
    id: page

    signal accountSelected(var account)
    signal openPage(var component, var properties)

    property alias listView: listView

    title: qsTr("Problems Detected")

    C.ScrollView {
        anchors.fill: parent

        ListView {
            id: listView
            width: page.width
            height: page.height
            header: Components.Heading {
                text: qsTr("Problems Detected")
                padding: Utils.AppSettings.mediumSpace
                width: parent.width
            }
            model: OTL.Application.problemManager.problems
            delegate: C.ItemDelegate {
                id: control

                width: parent.width
                text: {
                    switch (modelData.type) {
                    case OTL.Problem.AccountSecretsMissing:
                        // The secrets for an account are missing:
                        let account = modelData.contextObject
                        let msg = qsTr("Missing secrets for account")
                        msg += " <strong>" + account.name + "</strong>"
                        return msg
                    case OTL.Problem.SyncFailed:
                        let library = modelData.contextObject
                        msg = qsTr("Synchronization failed for library")
                        msg += " <strong>" + library.name + "</strong>: " + modelData.message
                        return msg
                    default:
                        console.warn("Unknown problem type", modelData.type)
                        return "Unknown Problem"
                    }
                }
                contentItem: C.Label {
                    rightPadding: control.spacing + quickSolutionButton.width
                    text: control.text
                    font: control.font
                    verticalAlignment: Text.AlignVCenter
                }
                onClicked: {
                    switch (modelData.type) {
                    case OTL.Problem.AccountSecretsMissing:
                        let accountTypeName = OTL.Application.accountTypeToString(
                                modelData.contextObject.type)
                        page.openPage(
                                    Qt.resolvedUrl(
                                        "./Edit" + accountTypeName + "AccountPage.qml"),
                                    {
                                        "account": OTL.Application.loadAccount(
                                                       modelData.contextObject.uid) // Load a copy of the account to ensure we have latest secrets
                                    })
                        break
                    case OTL.Problem.SyncFailed:
                        page.openPage(Qt.resolvedUrl("./LibraryPage.qml"), {
                                          "library": modelData.contextObject
                                      })
                        break
                    default:
                        console.warn("Unknown problem type", modelData.type)
                    }
                }

                C.Button {
                    id: quickSolutionButton
                    anchors.right: parent.right
                    anchors.rightMargin: control.rightPadding
                    text: {
                        switch (modelData.type) {
                        case OTL.Problem.SyncFailed:
                            return qsTr("Retry Sync")
                        case OTL.Problem.AccountSecretsMissing:
                            return "Load Secrets"
                        default:
                            return ""
                        }
                    }
                    onClicked: {
                        switch (modelData.type) {
                        case OTL.Problem.AccountSecretsMissing:
                            OTL.Application.loadSecretsForAccounts(
                                        modelData.contextObject)
                            break
                        case OTL.Problem.SyncFailed:
                            OTL.Application.syncLibrary(modelData.contextObject)
                            break
                        }
                    }

                    visible: text !== ""
                }
            }
        }
    }
}
