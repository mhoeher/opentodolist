import QtQuick 2.0
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.1
import QtQml 2.2

import OpenTodoList 1.0
import OpenTodoList.UI 1.0

Item {
    id: root

    property var model: ItemsModel {}
    property bool allowEntryCreation: false

    signal todoSelected(Todo todo)
    signal addEntry(string title)

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
        Label {
            text: confirmDeleteTodoDialog.todo ? qsTr("Do you want to delete the todo <strong>%1</strong>? " +
                              "This cannot be undone.").arg(confirmDeleteTodoDialog.todo.displayTitle) : ""
            wrapMode: Text.WordWrap
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
            id: todoItem

            width: parent.width
            height: row.height + Globals.defaultMargin

            Rectangle {
                width: parent.width * (object.percentageDone / 100.0)
                height: parent.height
                visible: object.percentageDone > 0
                color: Colors.highlight
                opacity: 0.1
            }

            ReorderWeightedItem {
                anchors.fill: parent
                onClicked: todoSelected(object)
                model: root.model
            }

            GridLayout {
                id: row

                property date date: object.dueTo
                property bool hasDate: date.getTime() === date.getTime()

                width: parent.width
                anchors.verticalCenter: parent.verticalCenter
                columns: 4

                Symbol {
                    id: checkMark

                    symbol: object.done ? Fonts.symbols.faCheckSquareO : Fonts.symbols.faSquareO
                    onClicked: object.done = !object.done
                }

                URLText {
                    plainText: object.displayTitle
                    Layout.fillWidth: true
                    Layout.columnSpan: 2
                    wrapMode: Text.WordWrap
                }

                Symbol {
                    symbol: Fonts.symbols.faTrashO
                    onClicked: {
                        confirmDeleteTodoDialog.todo = object;
                        confirmDeleteTodoDialog.open();
                    }
                }

                Item {
                    width: 1
                    height: 1
                    visible: row.hasDate
                }

                Symbol {
                    symbol: Fonts.symbols.faCalendar
                    visible: row.hasDate
                    enabled: false
                }

                Label {
                    text: row.date.toLocaleDateString()
                    Layout.fillWidth: true
                    visible: row.hasDate
                    enabled: false
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
                    placeholderText: qsTr("Add Todo...")
                    background: Item {}
                    onAccepted: newEntryButton.clicked(null)
                    selectByMouse: true
                }

                Symbol {
                    id: newEntryButton
                    symbol: Fonts.symbols.faPlus
                    onClicked: {
                        if (newEntryEdit.displayText !== "") {
                            root.addEntry(newEntryEdit.displayText);
                            delayedClearTimer.start();
                        }
                    }
                }
            }

            Rectangle {
                width: parent.width
                height: 1
                color: background.border.color
            }

            Timer {
                id: delayedClearTimer
                interval: 10
                repeat: false
                onTriggered: {
                    newEntryEdit.clear();
                    newEntryEdit.forceActiveFocus();
                }
            }
        }
    }

    Rectangle {
        id: background
        border {
            width: 1
            color: Colors.mid
        }
        color: Colors.midlight
        anchors.fill: parent
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
        interactive: false
    }
}
