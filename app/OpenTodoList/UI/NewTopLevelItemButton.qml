import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Controls.Material 2.2

import OpenTodoList.UI 1.0

RoundButton {
    id: newItemButton

    signal newTodoList()
    signal newImage()
    signal newNote()

    anchors {
        right: parent.right
        bottom: parent.bottom
        margins: Globals.defaultMargin * 2
    }
    width: Globals.minButtonHeight * 2
    height: Globals.minButtonHeight * 2
    radius: Globals.minButtonHeight
    text: "+"
    font.pixelSize: height * 0.6
    onClicked: {
        newItemMenu.parent = parent;
        if (newItemMenu.visible) {
            newItemMenu.close();
        } else {
            newItemMenu.open();
        }
    }
    Material.background: Material.Green

    Menu {
        id: newItemMenu

        y: newItemButton.y - height
        x: newItemButton.x - width / 2

        MenuItem {
            text: qsTr("Note")
            onTriggered: newItemButton.newNote()
        }
        MenuItem {
            text: qsTr("Todo List")
            onTriggered: newItemButton.newTodoList()
        }
        MenuItem {
            text: qsTr("Image")
            onTriggered: newItemButton.newImage()
        }
    }
}
