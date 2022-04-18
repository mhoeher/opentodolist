import QtQuick 2.4
import QtQuick.Layouts 1.0

import "../Components" as Components
import "../Utils" as Utils
import "../Controls" as C

import OpenTodoList 1.0 as OTL

C.Page {
    id: page

    property alias buttons: buttonBox
    property OTL.Account account
    property alias busyIndicator: busyIndicator
    property alias noLibrariesFoundLabel: noLibrariesFoundLabel
    property alias searchingForLibrariesLabel: searchingForLibrariesLabel
    property alias accountsView: accountsView
    property alias buttonGroup: buttonGroup
    property alias createNewLibraryButton: createNewLibraryButton
    property alias newLibraryNameEdit: newLibraryNameEdit

    signal closePage
    signal libraryCreated(var library)

    footer: C.DialogButtonBox {
        id: buttonBox
        standardButtons: C.DialogButtonBox.Ok | C.DialogButtonBox.Cancel
        onRejected: page.closePage()
        onAccepted: {
            if (createNewLibraryButton.checked) {
                var lib = OTL.Application.addNewLibraryToAccount(
                            page.account, newLibraryNameEdit.displayText)
            } else {
                lib = OTL.Application.addExistingLibraryToAccount(
                            page.account,
                            buttonGroup.checkedButton.existingAccount)
            }
            if (lib) {
                page.libraryCreated(lib)
            }
        }
    }

    onAccountChanged: account.findExistingLibraries()

    QtObject {
        id: d

        property var okButton: buttons.standardButton(C.DialogButtonBox.Ok)

        Component.onCompleted: page.account.findExistingLibraries()
    }

    Binding {
        target: d.okButton
        property: "enabled"
        value: buttonGroup.checkedButton
               && (buttonGroup.checkedButton != createNewLibraryButton
                   || newLibraryNameEdit.displayText !== "")
    }

    C.ButtonGroup {
        id: buttonGroup

        exclusive: true
    }

    C.ScrollView {
        anchors.fill: parent
        padding: Utils.AppSettings.mediumSpace

        GridLayout {
            id: grid

            width: page.width - 2 * 10
            columns: 2
            implicitWidth: childrenRect.width
            implicitHeight: childrenRect.height

            Components.Heading {
                text: qsTr(
                          "Create Library in Account") + " " + page.account.name
                Layout.fillWidth: true
                Layout.columnSpan: 2
            }

            C.Label {
                Layout.maximumWidth: grid.width
                Layout.columnSpan: 2
                text: qsTr(
                          "A library created in an account is synchronized with it. "
                          + "This allows to easily back up a library to a server and later on "
                          + "restore it from there. Additionally, such libraries can be shared "
                          + "with other users (if the server allows this).")
            }

            Components.Heading {
                text: qsTr("Existing Libraries")
                level: 2
                Layout.columnSpan: 2
            }

            C.Label {
                Layout.maximumWidth: grid.width
                Layout.columnSpan: 2
                text: qsTr("Select an existing library on the server to add it to the app.")
            }

            C.Label {
                id: noLibrariesFoundLabel

                text: qsTr("No libraries were found on the server.")
                Layout.columnSpan: 2
                visible: page.account.remoteLibraries.length === 0
                         && !page.account.findingRemoteLibraries
            }

            C.Label {
                id: searchingForLibrariesLabel

                text: qsTr("Searching existing libraries...")
                Layout.columnSpan: 2
                visible: page.account.findingRemoteLibraries
            }

            C.BusyIndicator {
                id: busyIndicator
                Layout.columnSpan: 2
                Layout.alignment: Qt.AlignHCenter
                visible: page.account.findingRemoteLibraries
            }

            Column {
                Layout.fillWidth: true
                Layout.columnSpan: 2

                Repeater {
                    id: accountsView

                    delegate: C.RadioButton {
                        id: itemDelegate

                        property var existingAccount: modelData

                        text: modelData.name
                        width: parent.width
                        C.ButtonGroup.group: buttonGroup
                    }
                    model: {
                        let result = []
                        page.account.remoteLibraries.forEach(remoteLib => {
                                                                 if (!OTL.Application.libraryExists(
                                                                         remoteLib.uid)) {
                                                                     result.push(
                                                                         remoteLib)
                                                                 }
                                                             })
                        return result
                    }
                }
            }

            Components.Heading {
                text: qsTr("Create a New Library")
                level: 2
                Layout.columnSpan: 2
            }

            C.Label {
                Layout.maximumWidth: grid.width
                Layout.columnSpan: 2
                text: qsTr(
                          "Create a new library, which will be synchronized with the server. "
                          + "Such a library can be added to the app on other devices as well to "
                          + "synchronize data.")
            }

            C.RadioButton {
                id: createNewLibraryButton

                C.ButtonGroup.group: buttonGroup
            }

            C.TextField {
                id: newLibraryNameEdit
                placeholderText: qsTr("My new library's name")
                Layout.fillWidth: true
                onFocusChanged: {
                    if (focus) {
                        createNewLibraryButton.checked = true
                    }
                }
            }
        }
    }
}
