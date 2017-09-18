import QtQuick 2.0
import QtQuick.Controls 2.2

import OpenTodoList 1.0

Dialog {
    id: dialog
    
    property TopLevelItem __item
    
    
    function addTag(item) {
        __item = item;
        edit.text = "";
        open();
        edit.forceActiveFocus();
    }
    
    title: qsTr("Add tag...")
    standardButtons: Dialog.Ok | Dialog.Cancel
    
    onAccepted: if (edit.text !== "") __item.addTag(edit.text)
    
    TextField {
        id: edit
        placeholderText: qsTr("Enter tag name...")
        width: parent ? parent.width : implicitWidth
    }
    
}
