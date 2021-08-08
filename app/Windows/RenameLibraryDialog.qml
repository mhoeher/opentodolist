import QtQuick 2.10

import OpenTodoList 1.0 as OTL

import "../Components" as Components
import "../Controls" as C


CenteredDialog {
    id: dialog

    function renameLibrary(item) {
        d.library = item;
        edit.text = item.name;
        dialog.open();
        edit.forceActiveFocus();
    }

    standardButtons: C.Dialog.Ok | C.Dialog.Cancel
    title: qsTr("Rename Library")
    width: idealDialogWidth

    onAccepted: {
        if (edit.text !== "") {
            d.library.name = edit.displayText;
        }
    }

    QtObject {
        id: d

        property OTL.Library library
    }

    C.TextField {
        id: edit
        placeholderText: qsTr("Enter library title...")
        onAccepted: dialog.accept()
        width: parent.width
    }
}
