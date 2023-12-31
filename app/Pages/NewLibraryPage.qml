import QtQuick 2.10
import QtQuick.Layouts 1.1

import "../Components" as Components
import "../Controls" as C
import "../Windows"
import "../Fonts"
import "../Utils" as Utils

import OpenTodoList 1.0 as OTL

C.Page {
    id: page
    property alias addLocalLibraryDelegate: addLocalLibraryDelegate
    property alias addFolderAsLibraryDelegate: addFolderAsLibraryDelegate
    property alias accountsList: accountsList
    property alias addAccountButton: addAccountButton

    signal accountSelected(var account)
    signal openPage(var component, var properties)
    signal libraryCreated(var library)

    title: qsTr("Create Library")
    onAccountSelected: account => page.openPage(newLibaryFromAccountPage, {
                                                    "account": account
                                                })

    Component {
        id: newLocalLibraryPage

        NewLocalLibraryPage {
            onLibraryCreated: page.libraryCreated(library)
        }
    }

    Component {
        id: newLibraryInFolderPage

        NewLibraryInFolderPage {
            onLibraryCreated: page.libraryCreated(library)
        }
    }

    Component {
        id: newLibaryFromAccountPage

        NewLibraryFromAccountPage {
            onLibraryCreated: page.libraryCreated(library)
        }
    }

    Utils.Accounts {
        id: accountsModel
    }

    C.ScrollView {
        anchors.fill: parent

        Column {
            width: page.width

            Components.Heading {
                text: qsTr("Create Library")
                padding: Utils.AppSettings.mediumSpace
                width: parent.width
            }

            C.ItemDelegate {
                id: addLocalLibraryDelegate
                text: qsTr("Local Library")
                width: parent.width
                onClicked: page.openPage(newLocalLibraryPage, {})
            }

            C.ItemDelegate {
                id: addFolderAsLibraryDelegate
                text: qsTr("Use Folder as Library")
                width: parent.width
                onClicked: page.openPage(newLibraryInFolderPage, {})


                /* On iOS, we currently have no notion of a "file system" in that sense. So
                  don't even show this to the user. */
                visible: Qt.platform.os !== "ios"
            }

            Components.Heading {
                level: 2
                text: qsTr("Add Libraries From Your Accounts")
                padding: Utils.AppSettings.mediumSpace
            }

            Repeater {
                id: accountsList

                model: accountsModel.accounts
                delegate: C.ItemDelegate {
                    id: accountDelegate

                    text: name
                    width: parent.width

                    Connections {
                        target: accountDelegate
                        function onClicked() {
                            page.accountSelected(account)
                        }
                    }
                }
            }

            C.Button {
                id: addAccountButton
                anchors.right: parent.right
                anchors.rightMargin: 10
                text: qsTr("Add Account")
                onClicked: page.openPage(Qt.resolvedUrl(
                                             "./AccountTypeSelectionPage.qml"),
                                         {
                                             "anchorPage": page
                                         })
            }
        }
    }
}
