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

        property AbstractButton okButton: null
        property bool isLibraryDir: OTL.Application.isLibraryDir(folderPathEdit.displayText)

        onIsLibraryDirChanged: {
            if (isLibraryDir) {
                nameEdit.text = OTL.Application.libraryNameFromDir(folderPathEdit.displayText);
            } else {
                nameEdit.clear();
            }
        }
    }

    Binding {
        target: d.okButton
        property: "enabled"
        value: OTL.Application.directoryExists(folderPathEdit.displayText) && !d.isLibraryDir |
               nameEdit.displayText !== ""
    }

    FolderSelectionDialog {
        id: selectFolder
        onAccepted: folderPathEdit.text = OTL.Application.urlToLocalFile(
                        selectFolder.folder)
        folder: StandardPaths.writableLocation(StandardPaths.HomeLocation)
    }
}
