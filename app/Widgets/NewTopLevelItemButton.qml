import QtQuick 2.10
import QtQuick.Controls 2.12

import "../Components"
import "../Fonts"
import "../Utils"

RoundButton {
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
        margins: 10
    }
    symbol: Icons.faPlus
    backgroundColor: Colors.positiveColor
    onClicked: {
        newItemMenu.parent = parent;
        if (newItemMenu.visible) {
            newItemMenu.close();
        } else {
            newItemMenu.open();
        }
    }

    Menu {
        id: newItemMenu

        y: newItemButton.y - height
        x: newItemButton.x - width / 2
        modal: true

        MenuItem {
            text: qsTr("Todo List")
            onTriggered: newItemButton.newTodoList()
        }
        MenuItem {
            id: newTodoMenuItem
            text: qsTr("Todo")
            onTriggered: newItemButton.newTodo()
            visible: newItemButton.createTodos
            height: visible ? implicitHeight : 0
        }
        MenuItem {
            text: qsTr("Note")
            onTriggered: newItemButton.newNote()
        }
        MenuItem {
            text: qsTr("Image")
            onTriggered: newItemButton.newImage()
            visible: newItemButton.createImages
            height: visible ? implicitHeight : 0
        }
    }
}
