import QtQuick 2.4
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.0

import Components 1.0 as Components
import OpenTodoList 1.0 as OTL

import "../Utils" as Utils

Page {
    id: page

    property alias buttons: buttonBox
    property var account: ({
                               "name": "user@host"
                           })
    property alias busyIndicator: busyIndicator
    property alias noLibrariesFoundLabel: noLibrariesFoundLabel
    property alias searchingForLibrariesLabel: searchingForLibrariesLabel
    property alias accountsView: accountsView
    property alias buttonGroup: buttonGroup
    property alias createNewLibraryButton: createNewLibraryButton
    property alias newLibraryNameEdit: newLibraryNameEdit


    signal closePage()
    signal libraryCreated(OTL.Library library)

    footer: DialogButtonBox {
        id: buttonBox
        standardButtons: DialogButtonBox.Ok | DialogButtonBox.Cancel
        onRejected: page.closePage()
        onAccepted: {
            if (createNewLibraryButton.checked) {
                var lib = OTL.Application.addNewLibraryToAccount(
                            page.account, newLibraryNameEdit.displayText);
            } else {
                lib = OTL.Application.addExistingLibraryToAccount(
                            page.account, buttonGroup.checkedButton.existingAccount);
            }
            if (lib) {
                page.libraryCreated(lib);
            }
        }
    }

    QtObject {
        id: d

        property OTL.Synchronizer sync: page.account.createSynchronizer()

        property var okButton: buttons.standardButton(DialogButtonBox.Ok)

        Component.onCompleted: sync.findExistingLibraries()
    }

    Binding {
        target: d.okButton
        property: "enabled"
        value: buttonGroup.checkedButton &&
               (buttonGroup.checkedButton !== createNewLibraryButton ||
                 newLibraryNameEdit.displayText !== "")
    }

    Connections {
        target: d.sync
        function onFindingLibrariesChanged() {
            if (!d.sync.findingLibraries) {
                var foundLibraries = [];
                for (var i = 0; i < d.sync.existingLibraries.length; ++i) {
                    var existingLib = d.sync.existingLibraries[i];
                    if (!OTL.Application.libraryExists(existingLib.uid)) {
                        foundLibraries.push(existingLib);
                    }
                }

                page.accountsView.model = foundLibraries;

                noLibrariesFoundLabel.visible = foundLibraries.length === 0;
            }
        }
    }

    ButtonGroup {
        id: buttonGroup

        exclusive: true
    }

    ScrollView {
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

            Label {
                Layout.maximumWidth: grid.width
                Layout.columnSpan: 2
                text: qsTr(
                          "A library created in an account is synchronized with it. "
                          + "This allows to easily back up a library to a server and later on "
                          + "restore it from there. Additionally, such libraries can be shared "
                          + "with other users (if the server allows this).")
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
            }

            Components.Heading {
                text: qsTr("Existing Libraries")
                level: 2
                Layout.columnSpan: 2
            }

            Label {
                Layout.maximumWidth: grid.width
                Layout.columnSpan: 2
                text: qsTr("Select an existing library on the server to add it to the app.")
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
            }

            Label {
                id: noLibrariesFoundLabel

                text: qsTr("No libraries were found on the server.")
                Layout.columnSpan: 2
                visible: false
            }

            Label {
                id: searchingForLibrariesLabel

                text: qsTr("Searching existing libraries...")
                Layout.columnSpan: 2
                visible: d.sync.findingLibraries
            }

            BusyIndicator {
                id: busyIndicator
                Layout.columnSpan: 2
                Layout.alignment: Qt.AlignHCenter
                visible: d.sync.findingLibraries
            }

            Column {
                Layout.fillWidth: true
                Layout.columnSpan: 2

                Repeater {
                    id: accountsView

                    delegate: RadioButton {
                        id: itemDelegate

                        property var existingAccount: modelData

                        text: modelData.name
                        width: parent.width
                        ButtonGroup.group: buttonGroup
                    }
                    onModelChanged: console.warn(accountsView.model)
                    model: []
                }
            }

            Components.Heading {
                text: qsTr("Create a New Library")
                level: 2
                Layout.columnSpan: 2
            }

            Label {
                Layout.maximumWidth: grid.width
                Layout.columnSpan: 2
                text: qsTr(
                          "Create a new library, which will be synchronized with the server. "
                          + "Such a library can be added to the app on other devices as well to "
                          + "synchronize data.")
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
            }

            RadioButton {
                id: createNewLibraryButton

                ButtonGroup.group: buttonGroup
            }

            TextField {
                id: newLibraryNameEdit
                placeholderText: qsTr("My new library's name")
                Layout.fillWidth: true
                onFocusChanged: {
                    if (focus) {
                        createNewLibraryButton.checked = true;
                    }
                }
            }
        }
    }
}
