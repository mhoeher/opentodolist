import QtQuick 2.14
import QtQuick.Controls.Material 2.0

import "../Controls" as C
import "../Components" as Cmp
import "../Utils" as Utils

import OpenTodoList 1.0 as OTL

C.Page {
    id: page

    padding: 0

    C.ScrollView {
        id: scrollView

        anchors.fill: parent
        padding: Utils.AppSettings.mediumSpace

        Flickable {
            id: flickable
            width: scrollView.availableWidth
            height: page.availableHeight
            contentHeight: column.childrenRect.height

            Utils.Accounts {
                id: accounts
            }

            Column {
                id: column

                width: flickable.width

                Cmp.Heading {
                    text: qsTr("Libraries")
                    width: parent.width
                }

                Repeater {
                    model: OTL.LibrariesModel {
                        cache: OTL.Application.cache
                    }
                    delegate: C.ItemDelegate {
                        text: name
                        width: parent.width
                        leftPadding: Utils.AppSettings.largeSpace
                        onClicked: page.C.StackView.view.push(
                                       Qt.resolvedUrl("./LibraryPage.qml"), {
                                           "library": library
                                       })
                        Rectangle {
                            height: parent.height
                            width: Utils.AppSettings.mediumSpace
                            color: library.color
                        }
                    }
                }

                C.Button {
                    text: qsTr("Add a new library")
                    anchors.right: parent.right
                    onClicked: {
                        let newLibraryPage = page.C.StackView.view.push(
                                Qt.resolvedUrl("./NewLibraryPage.qml"))
                        newLibraryPage.libraryCreated.connect(
                                    function (library) {
                                        console.warn("onLibraryCreated")
                                        let stack = page.C.StackView.view
                                        stack.pop(page)
                                        stack.push(Qt.resolvedUrl(
                                                       "./LibraryPage.qml"), {
                                                       "library": library
                                                   })
                                    })
                    }
                }

                Cmp.Heading {
                    text: qsTr("Accounts")
                    width: parent.width
                }

                Repeater {
                    model: accounts.accounts
                    delegate: C.ItemDelegate {
                        text: name
                        width: parent.width

                        onClicked: page.C.StackView.view.push(
                                       Qt.resolvedUrl(
                                           "Edit" + OTL.Application.accountTypeToString(
                                               account.type) + "AccountPage.qml"),
                                       {
                                           "account": account
                                       })
                    }
                }

                C.Button {
                    anchors.right: parent.right
                    text: qsTr("Add an account")
                    onClicked: page.C.StackView.view.push(
                                   Qt.resolvedUrl(
                                       "./AccountTypeSelectionPage.qml"), {
                                       "anchorPage": page
                                   })
                }
            }
        }
    }
}
