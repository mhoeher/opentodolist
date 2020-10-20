import QtQuick 2.10
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.12

import Components 1.0 as Components
import OpenTodoList 1.0 as OTL

import "../Windows"
import "../Fonts"
import Utils 1.0 as Utils

Page {
    id: page
    property alias addLocalLibraryDelegate: addLocalLibraryDelegate
    property alias addFolderAsLibraryDelegate: addFolderAsLibraryDelegate
    property alias accountsList: accountsList
    property alias addAccountButton: addAccountButton

    signal accountSelected(var account)
    signal openPage(var component, var properties)
    signal libraryCreated(OTL.Library library)

    title: qsTr("Create Library")
    onAccountSelected: page.openPage(newLibaryFromAccountPage, {account: account })

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

    ScrollView {
        anchors.fill: parent

        Column {
            width: page.width

            Components.Heading {
                text: qsTr("Create Library")
                padding: 10
                width: parent.width
            }

            ItemDelegate {
                id: addLocalLibraryDelegate
                text: qsTr("Local Library")
                width: parent.width
                onClicked: page.openPage(newLocalLibraryPage, {})
            }

            ItemDelegate {
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
                padding: 10
            }

            Repeater {
                id: accountsList

                model: accountsModel.accounts
                delegate: ItemDelegate {
                    id: accountDelegate

                    text: name
                    width: parent.width

                    Connections {
                        target: accountDelegate
                        function onClicked() {
                            page.accountSelected(account);
                        }
                    }
                }
            }

            Button {
                id: addAccountButton
                anchors.right: parent.right
                anchors.rightMargin: 10
                text: qsTr("Add Account")
                onClicked: page.openPage(Qt.resolvedUrl("./AccountTypeSelectionPage.qml"), {
                                                              anchorPage: page
                                                              });
            }
        }
    }
}
