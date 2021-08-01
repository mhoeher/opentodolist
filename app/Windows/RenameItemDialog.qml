import QtQuick 2.0

import OpenTodoList 1.0 as OTL

import "../Components" as Components
import "../Controls" as C


CenteredDialog {
    id: dialog
    
    property OTL.LibraryItem item
    
    function renameItem(item) {
        dialog.item = item;
        edit.text = item.title;
        dialog.open();
        edit.forceActiveFocus();
    }
    
    standardButtons: C.Dialog.Ok | C.Dialog.Cancel
    title: qsTr("Rename Item")
    width: idealDialogWidth
    
    onAccepted: {
        if (edit.text !== "") {
            item.title = edit.displayText;
        }
    }
    
    C.TextField {
        id: edit
        placeholderText: qsTr("Enter item title...")
        onAccepted: dialog.accept()
        width: dialog.availableWidth
    }
}
