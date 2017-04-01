import QtQuick 2.0
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.1
import QtQuick.Dialogs 1.2

import net.rpdev.OpenTodoList 1.0
import net.rpdev.OpenTodoList.UI 1.0

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

    MessageDialog {
        id: confirmDeleteTodoDialog

        property Todo todo

        title: qsTr("Delete Todo?")
        text: todo ? qsTr("Do you want to delete the todo <strong>%1</strong>? " +
                          "This cannot be undone.").arg(todo.title) : ""
        standardButtons: StandardButton.Ok | StandardButton.Cancel
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

                Label {
                    text: object.title
                    Layout.fillWidth: true
                    wrapMode: Text.WrapAnywhere
                }

                Symbol {
                    symbol: Fonts.symbols.faTrashO
                    onClicked: {
                        confirmDeleteTodoDialog.todo = todo;
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
                    style: TextFieldStyle {
                        background: Item {}
                        textColor: "black"
                    }

                    onAccepted: {
                        if (displayText !== "") {
                            root.addEntry(displayText, false);
                            text = "";
                            forceActiveFocus();
                        }
                    }
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
        source: "qrc:/net/rpdev/OpenTodoList/UI/soft_wallpaper/soft_wallpaper.png"
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
