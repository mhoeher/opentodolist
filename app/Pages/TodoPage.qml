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

    title: OTL.Application.htmlToPlainText(Markdown.format(item.title))

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
                var task = page.item.addTask();
                task.title = newTaskEdit.text;
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
            container: page.library.tasks
        }
        todo: page.item.uid
        searchString: filterBar.text
    }

    TextInputBar {
        id: filterBar
        placeholderText: qsTr("Search term 1, search term 2, ...")
        symbol: Icons.faTimes
        showWhenNonEmpty: true
        closeOnButtonClick: true
    }

    Pane {
        id: background

        backgroundColor: Colors.color(Colors.itemColor(page.todoList),
                                      Colors.shade50)
        anchors.fill: parent

        ScrollView {
            id: scrollView

            anchors.fill: parent

            Column {
                width: scrollView.width

                TodosWidget {
                    width: parent.width
                    model: tasks
                    title: qsTr("Tasks")
                    decorativeSymbol: Icons.faCircle
                    decorativeSymbolFont: Fonts.icons
                    symbol: Icons.faPlus
                    onHeaderButtonClicked: newTaskDialog.open()
                }

                ItemNotesEditor {
                    item: page.item
                    width: parent.width
                }

                ItemDueDateEditor {
                    item: page.item
                    width: parent.width
                }

                Attachments {
                    item: page.item
                    width: parent.width
                }
            }
        }
    }
}

