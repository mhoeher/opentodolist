import QtQuick 2.4
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.0
import Qt.labs.platform 1.1

import Components 1.0 as Components
import OpenTodoList 1.0 as OTL

import "../Windows"
import "../Utils" as Utils

Page {
    id: page

    property alias buttons: buttonBox
    property alias nameEdit: nameEdit
    property alias folderPathEdit: folderPathEdit
    property alias openFolderButton: openFolderButton

    signal closePage()
    signal libraryCreated(OTL.Library library)

    footer: DialogButtonBox {
        id: buttonBox
        standardButtons: DialogButtonBox.Ok | DialogButtonBox.Cancel
        onRejected: page.closePage()
    }
    Component.onCompleted: d.okButton = buttons.standardButton(DialogButtonBox.Ok)

    QtObject {
        id: d

        property var okButton: null
        property bool isLibraryDir: OTL.Application.isLibraryDir(
                                        OTL.Application.localFileToUrl(folderPathEdit.displayText))

        onIsLibraryDirChanged: {
            if (isLibraryDir) {
                nameEdit.text = OTL.Application.libraryNameFromDir(
                            OTL.Application.localFileToUrl(folderPathEdit.displayText));
            } else {
                nameEdit.clear();
            }
        }
    }

    Binding {
        target: d.okButton
        property: "enabled"
        value: OTL.Application.directoryExists(folderPathEdit.displayText) && !d.isLibraryDir ||
               nameEdit.displayText !== ""
    }

    Connections {
        target: d.okButton
        function onClicked() {
            var lib = OTL.Application.addLibraryDirectory(folderPathEdit.displayText);
            if (lib) {
                if (nameEdit.enabled) {
                    lib.name = nameEdit.displayText;
                }
                page.libraryCreated(lib);
            }
        }
    }

    FolderSelectionDialog {
        id: selectFolder
        onAccepted: folderPathEdit.text = OTL.Application.urlToLocalFile(
                        selectFolder.folder)
        folder: StandardPaths.writableLocation(StandardPaths.HomeLocation)
    }

    ScrollView {
        anchors.fill: parent
        padding: Utils.AppSettings.mediumSpace

        GridLayout {
            id: grid

            width: page.width - 2 * 10
            columns: 3
            implicitWidth: childrenRect.width
            implicitHeight: childrenRect.height

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
                onClicked: selectFolder.open()
            }

            Label {
                text: qsTr("Name:")
            }

            TextField {
                id: nameEdit
                placeholderText: qsTr("My Local Library Name")
                Layout.fillWidth: true
                Layout.columnSpan: 2
                enabled: !d.isLibraryDir
            }
        }
    }
}
