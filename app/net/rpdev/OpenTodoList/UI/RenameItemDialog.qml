import QtQuick 2.0
import QtQuick.Dialogs 1.2
import QtQuick.Controls 1.4

import net.rpdev.OpenTodoList 1.0

Dialog {
    id: dialog
    
    property BasicItem __item
    
    function renameItem(item) {
        __item = item;
        edit.text = item.title;
        dialog.open();
        edit.forceActiveFocus();
    }
    
    standardButtons: StandardButton.Ok | StandardButton.Cancel
    title: qsTr("Rename Item")
    
    onAccepted: if (edit.text !== "") __item.title = edit.text
    
    TextField {
        id: edit
        placeholderText: qsTr("Enter item title...")
        width: parent ? parent.width : implicitWidth
    }
}
