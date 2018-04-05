import QtQuick 2.10

import OpenTodoList 1.0 as OTL

import "../Components"


CenteredDialog {
    id: dialog

    function renameLibrary(item) {
        d.library = item;
        edit.text = item.name;
        dialog.open();
        edit.forceActiveFocus();
    }

    standardButtons: Dialog.Ok | Dialog.Cancel
    title: qsTr("Rename Library")

    onAccepted: {
        if (edit.text !== "") {
            d.library.name = edit.text;
        }
    }

    QtObject {
        id: d

        property OTL.Library library
    }

    TextField {
        id: edit
        placeholderText: qsTr("Enter library title...")
        onAccepted: dialog.accept()
    }
}
