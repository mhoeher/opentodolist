import QtQuick 2.4
import QtQuick.Controls 2.5

import OpenTodoList 1.0 as OTL

NewLibraryFromAccountPageForm {
    id: page

    signal closePage()
    signal libraryCreated(OTL.Library library)

    buttons.onRejected: page.closePage()
    buttons.onAccepted: {
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

    searchingForLibrariesLabel.visible: d.sync.findingLibraries
    busyIndicator.visible: d.sync.findingLibraries
    accountsView.onModelChanged: console.warn(accountsView.model)
    accountsView.model: []
    noLibrariesFoundLabel.visible: false
    newLibraryNameEdit.onFocusChanged: {
        if (focus) {
            createNewLibraryButton.checked = true;
        }
    }

    QtObject {
        id: d

        property OTL.Synchronizer sync: page.account.createSynchronizer()

        property AbstractButton okButton: buttons.standardButton(DialogButtonBox.Ok)

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
        onFindingLibrariesChanged: {
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
}
