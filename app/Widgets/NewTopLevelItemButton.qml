import QtQuick 2.10
import QtQuick.Controls 2.12

import "../Components"
import "../Fonts"
import "../Utils"

RoundButton {
    id: newItemButton

    signal newTodoList()
    signal newImage()
    signal newNote()

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
