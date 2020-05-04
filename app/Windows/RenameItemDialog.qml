import QtQuick 2.0
import QtQuick.Controls 2.12

import OpenTodoList 1.0 as OTL

import "../Components" as Components


CenteredDialog {
    id: dialog
    
    property OTL.Item item
    
    function renameItem(item) {
        dialog.item = item;
        edit.text = item.title;
        dialog.open();
        edit.forceActiveFocus();
    }
    
    standardButtons: Dialog.Ok | Dialog.Cancel
    title: qsTr("Rename Item")
    width: idealDialogWidth
    
    onAccepted: {
        if (edit.text !== "") {
            item.title = edit.displayText;
        }
    }
    
    Components.TextField {
        id: edit
        placeholderText: qsTr("Enter item title...")
        onAccepted: dialog.accept()
        width: dialog.availableWidth
    }
}
