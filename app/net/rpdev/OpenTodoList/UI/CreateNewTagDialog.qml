import QtQuick 2.0
import QtQuick.Dialogs 1.2
import QtQuick.Controls 1.4

import net.rpdev.OpenTodoList 1.0

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
    standardButtons: StandardButton.Ok | StandardButton.Cancel
    
    onAccepted: if (edit.text !== "") __item.addTag(edit.text)
    
    onVisibleChanged: {
        // Hand back focus to main window:
        if (!visible) {
            Globals.appWindow.focus()
        }
    }
    
    TextField {
        id: edit
        placeholderText: qsTr("Enter tag name...")
        width: parent ? parent.width : implicitWidth
    }
    
}
