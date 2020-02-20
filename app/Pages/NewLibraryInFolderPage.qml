import QtQuick 2.4
import QtQuick.Controls 2.5
import Qt.labs.platform 1.1

import OpenTodoList 1.0 as OTL

import "../Windows"

NewLibraryInFolderPageForm {
    id: page

    signal closePage()
    signal libraryCreated(OTL.Library library)

    buttons.onRejected: page.closePage()
    Component.onCompleted: d.okButton = buttons.standardButton(DialogButtonBox.Ok)
    openFolderButton.onClicked: selectFolder.open()
    nameEdit.enabled: !d.isLibraryDir

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
        onClicked: {
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
}
