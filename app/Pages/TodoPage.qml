import QtQuick 2.5

import OpenTodoList 1.0 as OTL

import "../Components"
import "../Fonts"
import "../Windows"
import "../Widgets"
import "../Utils"

Page {
    id: page

    property OTL.Todo item: OTL.Todo {}
    property OTL.Library library: null
    property OTL.TodoList todoList: null

    signal closePage()
    signal openPage(var component, var properties)

    function deleteItem() {
        confirmDeleteDialog.deleteItem(item);
    }

    function renameItem() {
        renameItemDialog.renameItem(item);
    }

    function find() {
        filterBar.edit.forceActiveFocus()
    }

    function attach() {
        attachments.attach();
    }

    title: itemTitle.text

    MarkdownConverter {
        id: itemTitle
        markdown: item.title
        strip: true
    }

    DeleteItemDialog {
        id: confirmDeleteDialog
        onAccepted: {
            page.closePage();
        }
    }

    RenameItemDialog {
        id: renameItemDialog
    }

    CenteredDialog {
        id: newTaskDialog
        title: qsTr("Add Task")
        width: idealDialogWidth

        TextField {
            id: newTaskEdit

            width: newTaskDialog.availableWidth
            Keys.onEnterPressed: newTaskDialog.accept()
            Keys.onReturnPressed: newTaskDialog.accept()
        }

        standardButtons: Dialog.Ok | Dialog.Cancel

        onAccepted: {
            if (newTaskEdit.text !== "") {
                var properties = {
                    "title": newTaskEdit.text
                };
                var task = OTL.Application.addTask(
                            page.library,
                            page.item,
                            properties);
            }
            newTaskEdit.text = "";
        }
        onRejected: {
            newTaskEdit.text = "";
        }
        onOpened: {
            newTaskEdit.forceActiveFocus()
        }
    }

    OTL.ItemsSortFilterModel {
        id: tasks
        sourceModel: OTL.ItemsModel {
            cache: OTL.Application.cache
            parentItem: page.item.uid
            searchString: filterBar.text
        }
    }

    TextInputBar {
        id: filterBar
        placeholderText: qsTr("Search term 1, search term 2, ...")
        symbol: Icons.faTimes
        showWhenNonEmpty: true
        closeOnButtonClick: true
    }

    Pane {
        anchors.fill: parent
        backgroundColor: Colors.color(Colors.itemColor(item), Colors.shade50)
    }

    ScrollView {
        id: scrollView
        anchors {
            left: parent.left
            right: parent.right
            top: filterBar.bottom
            bottom: parent.bottom
        }

        Pane {
            id: background

            backgroundColor: Colors.color(Colors.itemColor(page.todoList),
                                          Colors.shade50)
            width: scrollView.width


            Column {
                width: parent.width

                ItemPageHeader {
                    item: page.item
                }

                ItemProgressEditor {
                    item: page.item
                    width: parent.width
                }

                ItemNotesEditor {
                    item: page.item
                    width: parent.width
                }

                TodosWidget {
                    width: parent.width
                    model: tasks
                    title: qsTr("Tasks")
                    headerItemVisible: false
                    onHeaderButtonClicked: newTaskDialog.open()
                }

                Attachments {
                    id: attachments
                    item: page.item
                    width: parent.width
                }

                Item {
                    width: 1
                    height: newItemButton.height
                }
            }
        }
    }

    NewItemButton {
        id: newItemButton

        onClicked: newTaskDialog.open()
    }
}

