import QtQuick 2.4
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.0

import Components 1.0 as Components

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

    footer: DialogButtonBox {
        id: buttonBox
        standardButtons: DialogButtonBox.Ok | DialogButtonBox.Cancel
    }

    ButtonGroup {
        id: buttonGroup

        exclusive: true
    }

    ScrollView {
        anchors.fill: parent
        padding: 10

        GridLayout {
            id: grid

            width: page.width - 2 * 10
            columns: 2

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
            }

            Label {
                id: searchingForLibrariesLabel

                text: qsTr("Searching existing libraries...")
                Layout.columnSpan: 2
            }

            BusyIndicator {
                id: busyIndicator
                Layout.columnSpan: 2
                Layout.alignment: Qt.AlignHCenter
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
            }
        }
    }
}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/

