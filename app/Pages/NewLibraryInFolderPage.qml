import QtQuick 2.4
import QtQuick.Layouts 1.0
import Qt.labs.platform 1.1

import "../Components" as Components
import "../Controls" as C
import "../Windows"
import "../Utils" as Utils

import OpenTodoList 1.0 as OTL

C.Page {
    id: page

    property alias buttons: buttonBox
    property alias nameEdit: nameEdit
    property alias folderPathEdit: folderPathEdit
    property alias openFolderButton: openFolderButton

    signal closePage
    signal libraryCreated(var library)

    footer: C.DialogButtonBox {
        id: buttonBox
        standardButtons: C.DialogButtonBox.Ok | C.DialogButtonBox.Cancel
        onRejected: page.closePage()
    }
    Component.onCompleted: d.okButton = buttons.standardButton(
                               C.DialogButtonBox.Ok)

    QtObject {
        id: d

        property var okButton: null
        property bool isLibraryDir: OTL.Application.isLibraryDir(
                                        OTL.Application.localFileToUrl(
                                            folderPathEdit.displayText))

        onIsLibraryDirChanged: {
            if (isLibraryDir) {
                nameEdit.text = OTL.Application.libraryNameFromDir(
                            OTL.Application.localFileToUrl(
                                folderPathEdit.displayText))
            } else {
                nameEdit.clear()
            }
        }
    }

    Binding {
        target: d.okButton
        property: "enabled"
        value: OTL.Application.directoryExists(folderPathEdit.displayText)
               && !d.isLibraryDir || nameEdit.displayText !== ""
    }

    Connections {
        target: d.okButton
        function onClicked() {
            var lib = OTL.Application.addLibraryDirectory(
                        folderPathEdit.displayText)
            if (lib) {
                if (nameEdit.enabled) {
                    lib.name = nameEdit.displayText
                }
                page.libraryCreated(lib)
            }
        }
    }

    FolderSelectionDialog {
        id: selectFolder

        title: qsTr("Select a Folder")
        onAccepted: folderPathEdit.text = OTL.Application.urlToLocalFile(
                        selectFolder.selectedFolder)
        selectedFolder: {
            switch (Qt.platform.os) {
            case "android":
                return "file://" + OTL.Application.getExternalFilesDir()
            default:
                return StandardPaths.writableLocation(
                            StandardPaths.HomeLocation)
            }
        }
    }

    C.ScrollView {
        anchors.fill: parent
        padding: Utils.AppSettings.mediumSpace

        GridLayout {
            id: grid

            width: page.width - 2 * Utils.AppSettings.mediumSpace
            columns: 3
            implicitWidth: width
            implicitHeight: childrenRect.height

            Components.Heading {
                text: qsTr("Open a Folder as a Library")
                Layout.fillWidth: true
                Layout.columnSpan: 3
            }

            C.Label {
                Layout.maximumWidth: grid.width
                Layout.columnSpan: 3
                text: qsTr("You can use any folder as a location for a library.<br/><br/>This is especially useful when you want to use another tool (like a sync client of a cloud provider) to sync your data with a server.")
            }

            C.Label {
                text: qsTr("Folder:")
            }

            C.TextField {
                id: folderPathEdit
                placeholderText: qsTr("Path to a folder to use as a library")
                Layout.fillWidth: true
            }

            C.Button {
                id: openFolderButton
                text: qsTr("Select")
                onClicked: selectFolder.open()
            }

            C.Label {
                text: qsTr("Name:")
            }

            C.TextField {
                id: nameEdit
                placeholderText: qsTr("My Local Library Name")
                Layout.fillWidth: true
                Layout.columnSpan: 2
                enabled: !d.isLibraryDir
            }
        }
    }
}
