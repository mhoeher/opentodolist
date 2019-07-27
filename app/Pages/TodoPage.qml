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

    readonly property bool editingNotes: itemNotesEditor.editing

    function finishEditingNotes() {
        itemNotesEditor.finishEditing();
    }

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

    function setDueDate(dueDate) {
        page.item.dueTo =  dueDate;
    }

    function getDueDate() {
        return page.item.dueTo;
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
        backgroundColor: Colors.color(
                             Colors.itemColor(page.todoList), Colors.shade50)
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
                    id: itemNotesEditor
                    item: page.item
                    width: parent.width
                }

                TodosWidget {
                    width: parent.width
                    model: tasks
                    title: qsTr("Tasks")
                    headerItemVisible: false
                    allowCreatingNewItems: true
                    newItemPlaceholderText: qsTr("Add new task...")
                    onCreateNewItem: {
                        var properties = {
                            "title": title
                        };
                        var task = OTL.Application.addTask(
                                    page.library,
                                    page.item,
                                    properties);
                    }
                }

                Attachments {
                    id: attachments
                    item: page.item
                    width: parent.width
                }
            }
        }
    }
}

