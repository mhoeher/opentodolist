import QtQuick 2.10
import QtQuick.Controls 2.3

import OpenTodoList 1.0
import OpenTodoList.UI 1.0

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

    onVisibleChanged: {
        if (!visible) {
            Globals.appWindow.focus();
        }
    }

    QtObject {
        id: d

        property Library library
    }

    TextField {
        id: edit
        placeholderText: qsTr("Enter library title...")
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
