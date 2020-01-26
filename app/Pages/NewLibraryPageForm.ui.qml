import QtQuick 2.10
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.12

import Components 1.0 as Components

Page {
    id: page
    property alias addLocalLibraryDelegate: addLocalLibraryDelegate
    property alias addFolderAsLibraryDelegate: addFolderAsLibraryDelegate
    property alias accountsList: accountsList
    property alias addAccountButton: addAccountButton

    signal accountSelected(var account)

    title: qsTr("Create Library")

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
            }

            ItemDelegate {
                id: addFolderAsLibraryDelegate
                text: qsTr("Use Folder as Library")
                width: parent.width
            }

            Components.Heading {
                level: 2
                text: qsTr("Synced Libraries")
                padding: 10
            }

            Repeater {
                id: accountsList

                model: ListModel {
                    ListElement {
                        name: "Account 1"
                    }
                    ListElement {
                        name: "Account 2"
                    }
                    ListElement {
                        name: "Account 3"
                    }
                }
                delegate: ItemDelegate {
                    id: accountDelegate

                    text: name

                    Connections {
                        target: accountDelegate
                        onClicked: page.accountSelected(objectData)
                    }
                }
            }

            Button {
                id: addAccountButton
                anchors.right: parent.right
                anchors.rightMargin: 10
                text: qsTr("Add Account")
            }
        }
    }
}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/

