import QtQuick 2.4
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.0

import Components 1.0 as Components

Page {
    id: page

    property alias buttons: buttonBox
    property alias nameEdit: nameEdit
    property alias folderPathEdit: folderPathEdit
    property alias openFolderButton: openFolderButton

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
            columns: 3

            Components.Heading {
                text: qsTr("Open a Folder as a Library")
                Layout.fillWidth: true
                Layout.columnSpan: 3
            }

            Label {
                Layout.maximumWidth: grid.width
                Layout.columnSpan: 3
                text: qsTr("You can use any folder as a location for a library.<br/><br/>This is especially useful when you want to use another tool (like a sync client of a cloud provider) to sync your data with a server.")
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
            }

            Label {
                text: qsTr("Folder:")
            }

            TextField {
                id: folderPathEdit
                placeholderText: qsTr("Path to a folder to use as a library")
                Layout.fillWidth: true
            }

            Button {
                id: openFolderButton
                text: qsTr("Select")
            }

            Label {
                text: qsTr("Name:")
            }

            TextField {
                id: nameEdit
                placeholderText: qsTr("My Local Library Name")
                Layout.fillWidth: true
                Layout.columnSpan: 2
            }
        }
    }
}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/

