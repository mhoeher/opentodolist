import QtQuick 2.4
import QtQuick.Controls 2.5

import OpenTodoList 1.0 as OTL

NewLocalLibraryPageForm {
    signal closePage()
    signal libraryCreated(OTL.Library library)

    buttons.onRejected: closePage()
    buttons.onAccepted: {
        if (nameEdit.displayText !== "") {
            var lib = OTL.Application.addLocalLibrary(nameEdit.displayText);
            if (lib) {
                libraryCreated(lib);
            }
        }
    }

    Component.onCompleted: {
        d.okButton = buttons.standardButton(DialogButtonBox.Ok);
    }

    QtObject {
        id: d

        property AbstractButton okButton
    }

    Binding {
        target: d.okButton
        property: "enabled"
        value: nameEdit.displayText !== ""
    }
}
