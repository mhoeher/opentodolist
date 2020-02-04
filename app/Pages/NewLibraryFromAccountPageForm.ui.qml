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

    footer: DialogButtonBox {
        id: buttonBox
        standardButtons: DialogButtonBox.Ok | DialogButtonBox.Cancel
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
                          + "with other users (if the server allows this).<br/><br/>"
                          + "If there already are some libraries on the server, they can simply "
                          + "be selected to add them to the app locally. Additionally, new "
                          + "libraries can be created at any time.")
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
            }

            Components.Heading {
                text: qsTr("Existing Libraries")
                level: 2
                Layout.columnSpan: 2
            }

            Label {
                text: qsTr("No libraries were found on the server.")
                Layout.columnSpan: 2
            }

            Label {
                text: qsTr("Searching existing libraries...")
                Layout.columnSpan: 2
            }

            BusyIndicator {
                Layout.columnSpan: 2
                Layout.alignment: Qt.AlignHCenter
            }

            ListView {
                model: dummyModel
                delegate: ItemDelegate {
                    text: name
                    width: parent.width
                }
                Layout.columnSpan: 2
            }

            Components.Heading {
                text: qsTr("Create a New Library")
                level: 2
                Layout.columnSpan: 2
            }

            TextField {
                placeholderText: qsTr("My new library's name")
                Layout.columnSpan: 2
                Layout.fillWidth: true
            }
        }
    }

    ListModel {
        id: dummyModel

        ListElement {
            name: "Library 1"
        }
        ListElement {
            name: "Library 2"
        }
    }
}
