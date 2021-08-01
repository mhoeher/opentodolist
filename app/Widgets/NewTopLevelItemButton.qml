import QtQuick 2.10

import "../Components"
import "../Controls" as C
import "../Fonts"
import "../Utils"

C.RoundButton {
    id: newItemButton

    property bool createTodos: false
    property bool createImages: true

    signal newTodoList()
    signal newImage()
    signal newNote()
    signal newTodo()

    anchors {
        right: parent.right
        bottom: parent.bottom
        margins: AppSettings.mediumSpace
    }
    symbol: Icons.mdiAdd
    backgroundColor: Colors.positiveColor
    onClicked: {
        newItemMenu.parent = parent;
        if (newItemMenu.visible) {
            newItemMenu.close();
        } else {
            newItemMenu.open();
        }
    }

    C.Menu {
        id: newItemMenu

        y: newItemButton.y - height
        x: newItemButton.x - width / 2
        modal: true

        C.MenuItem {
            text: qsTr("Todo List")
            onTriggered: newItemButton.newTodoList()
        }
        C.MenuItem {
            id: newTodoMenuItem
            text: qsTr("Todo")
            onTriggered: newItemButton.newTodo()
            visible: newItemButton.createTodos
            height: visible ? implicitHeight : 0
        }
        C.MenuItem {
            text: qsTr("Note")
            onTriggered: newItemButton.newNote()
        }
        C.MenuItem {
            text: qsTr("Image")
            onTriggered: newItemButton.newImage()
            visible: newItemButton.createImages
            height: visible ? implicitHeight : 0
        }
    }
}
