import QtQuick 2.0

import OpenTodoList 1.0 as OTL

import "../Components"


CenteredDialog {
    id: dialog
    
    property OTL.Item __item
    
    function renameItem(item) {
        __item = item;
        edit.text = item.title;
        dialog.open();
        edit.forceActiveFocus();
    }
    
    standardButtons: Dialog.Ok | Dialog.Cancel
    title: qsTr("Rename Item")
    
    onAccepted: {
        if (edit.text !== "") {
            __item.title = edit.text;
        }
    }
    
    TextField {
        id: edit
        placeholderText: qsTr("Enter item title...")
        onAccepted: dialog.accept()
    }
}
