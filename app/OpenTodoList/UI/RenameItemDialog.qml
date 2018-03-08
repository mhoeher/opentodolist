import QtQuick 2.0
import QtQuick.Controls 2.2

import OpenTodoList 1.0
import OpenTodoList.UI 1.0

CenteredDialog {
    id: dialog
    
    property BasicItem __item
    
    function renameItem(item) {
        __item = item;
        edit.text = item.title;
        dialog.parent = MainWindow.window.contentItem;
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
    
    onVisibleChanged: {
        if (!visible) {
            Globals.appWindow.focus();
        }
    }
    
    TextField {
        id: edit
        placeholderText: qsTr("Enter item title...")
        anchors {
            left: parent.left
            right: parent.right
            top: parent.top
            margins: Globals.defaultMargin
        }
        wrapMode: Text.WrapAtWordBoundaryOrAnywhere
        selectByMouse: Globals.selectTextWithMouse
        onAccepted: dialog.accept()
    }
}
