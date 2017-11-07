import QtQuick 2.0
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.1

import OpenTodoList 1.0
import OpenTodoList.UI 1.0

Item {
    id: root

    property var model: ItemsModel {}
    property bool allowEntryCreation: false

    signal todoSelected(Todo todo)
    signal addEntry(string title, bool openItem)

    function focusNewItemInput() {
        if (listView.headerItem) {
            listView.headerItem.focusInputItem();
        }
    }

    height: listView.contentHeight

    CenteredDialog {
        id: confirmDeleteTodoDialog

        property Todo todo

        title: qsTr("Delete Todo?")
        Text {
            text: confirmDeleteTodoDialog.todo ? qsTr("Do you want to delete the todo <strong>%1</strong>? " +
                              "This cannot be undone.").arg(confirmDeleteTodoDialog.todo.title) : ""
            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
            anchors {
                left: parent.left
                right: parent.right
                top: parent.top
                margins: Globals.defaultMargin
            }
        }
        standardButtons: Dialog.Ok | Dialog.Cancel
        onAccepted: todo.deleteItem()
    }

    Component {
        id: itemDelegate

        Item {
            width: parent.width
            height: row.height + Globals.defaultMargin * 2

            MouseArea {
                anchors.fill: parent
                onClicked: todoSelected(object)
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
                    plainText: object.title
                    Layout.fillWidth: true
                    wrapMode: Text.WrapAnywhere
                }

                Symbol {
                    symbol: Fonts.symbols.faTrashO
                    onClicked: {
                        confirmDeleteTodoDialog.todo = object;
                        confirmDeleteTodoDialog.open();
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

            function createItemAndOpen() {
                if (newEntryEdit.displayText !== "") {
                    root.addEntry(newEntryEdit.displayText, true);
                    newEntryEdit.text = "";
                    newEntryEdit.forceActiveFocus();
                }
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
                    placeholderText: qsTr("Add Todo...")
                    background: Item {}

                    onAccepted: {
                        if (displayText !== "") {
                            root.addEntry(displayText, false);
                            text = "";
                            forceActiveFocus();
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
            color: Colors.itemDelimiter
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
        header: root.allowEntryCreation ? headerDelegate : null
        clip: true
    }
}
