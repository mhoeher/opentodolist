import QtQuick 2.0
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.1

import OpenTodoList 1.0
import OpenTodoList.UI 1.0

Item {
    id: root

    property var model: null
    property bool allowNewEntryCreation: true

    signal addEntry(string title)

    function focusNewItemInput() {
        if (listView.headerItem) {
            listView.headerItem.focusInputItem();
        }
    }

    height: listView.contentHeight

    CenteredDialog {
        id: confirmDeleteTaskDialog

        property Task task

        title: qsTr("Delete Task?")

        Label {
            text: confirmDeleteTaskDialog.task ? qsTr("Do you want to delete the task <strong>%1</strong>? " +
                              "This cannot be undone.").arg(confirmDeleteTaskDialog.task.displayTitle) : ""
            anchors {
                left: parent.left
                right: parent.right
                top: parent.top
                margins: Globals.defaultMargin
            }
            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
        }
        standardButtons: Dialog.Ok | Dialog.Cancel
        onAccepted: task.deleteItem()
    }

    RenameItemDialog {
        id: renameItemDialog
    }

    Component {
        id: itemDelegate

        Item {
            width: parent.width
            height: row.height + Globals.defaultMargin * 2

            ReorderWeightedItem {
                anchors.fill: parent
                onClicked: renameItemDialog.renameItem(object)
                model: root.model
            }

            RowLayout {
                id: row

                width: parent.width
                anchors.verticalCenter: parent.verticalCenter

                Symbol {
                    symbol: object.done ? Fonts.symbols.faCheckSquareO : Fonts.symbols.faSquareO
                    onClicked: object.done = !object.done
                }

                URLText {
                    plainText: object.displayTitle
                    Layout.fillWidth: true
                    wrapMode: Text.WrapAnywhere
                }

                Symbol {
                    symbol: Fonts.symbols.faTrashO
                    onClicked: {
                        confirmDeleteTaskDialog.task = object;
                        confirmDeleteTaskDialog.open();
                    }
                }
            }

            Rectangle {
                width: parent.width
                height: 1
                color: background.border.color
            }
        }
    }

    Component {
        id: headerDelegate

        Item {

            function createItem() {
                newEntryEdit.accepted();
            }

            function focusInputItem() {
                newEntryEdit.forceActiveFocus();
            }

            width: parent.width
            height: row.height + Globals.defaultMargin * 2


            RowLayout {
                id: row

                width: parent.width
                anchors.verticalCenter: parent.verticalCenter

                Item {
                    width: Globals.minButtonHeight
                    height: Globals.minButtonHeight
                }

                TextField {
                    id: newEntryEdit
                    Layout.fillWidth: true
                    placeholderText: qsTr("Add Task...")
                    background: Item {}

                    onAccepted: {
                        if (displayText !== "") {
                            root.addEntry(displayText);
                            text = "";
                        }
                    }
                    selectByMouse: true
                }

                Symbol {
                    symbol: Fonts.symbols.faPlus
                    onClicked: newEntryEdit.accepted()
                }
            }

            Rectangle {
                width: parent.width
                height: 1
                color: background.border.color
            }
        }
    }

    Rectangle {
        id: background
        border {
            width: 1
            color: Colors.midlight
        }
        anchors.fill: parent
    }

    Image {
        id: backgroundImage
        anchors {
            fill: background
            margins: 1
        }
        source: "soft_wallpaper/soft_wallpaper.png"
        fillMode: Image.Tile
    }

    ListView {
        id: listView
        anchors {
            fill: parent
            leftMargin: Globals.defaultMargin
            rightMargin: Globals.defaultMargin
        }
        model: root.model
        delegate: itemDelegate
        header: allowNewEntryCreation ? headerDelegate : null
        clip: true
    }
}
