import QtQuick 2.10
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.12


import OpenTodoList 1.0 as OTL

import Components 1.0 as Components

import "../Utils" as Utils

Page {
    id: page

    signal accountSelected(var account)
    signal openPage(var component, var properties)

    property alias listView: listView

    title: qsTr("Problems Detected")

    ScrollView {
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
            delegate: ItemDelegate {
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
                    case "SyncFailed":
                        let library = modelData.contextObject;
                        msg = qsTr("Synchronization failed for library")
                        msg += " <strong>" + library.name + "</strong>: " + modelData.message;
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

                    case "SyncFailed":
                        page.openPage(Qt.resolvedUrl("./LibraryPage.qml"), {
                                          library: modelData.contextObject
                                      });
                        break;

                    default:
                        console.warn("Unknown problem type", modelData.type);
                    }
                }
            }
        }
    }
}
