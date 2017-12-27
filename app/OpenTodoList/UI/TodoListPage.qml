import QtQuick 2.5
import QtQuick.Controls 2.2

import OpenTodoList 1.0
import OpenTodoList.UI 1.0

import "LibraryPageLogic.js" as Logic

Page {
    id: page

    property TodoList item: TodoList {}
    property var library: null

    signal openPage(var component, var properties)


    function newTodo() {
        todos.focusNewItemInput();
        //newTodoBar.edit.forceActiveFocus();
        //newTodoBar.edit.text = "";
    }

    function cancel() {
    }

    function deleteItem() {
        confirmDeleteDialog.open();
    }

    function toggleDoneTodosVisible() {
        doneTodos.visible = !doneTodos.visible
    }

    function find() {
        filterBar.edit.forceActiveFocus()
    }

    CenteredDialog {
        id: confirmDeleteDialog
        title: qsTr("Delete Todo List?")
        Label {
            text: qsTr("Are you sure you want to delete the todo list <strong>%1</strong>? This action " +
                       "cannot be undone.").arg(item.displayTitle)
            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
            anchors {
                left: parent.left
                right: parent.right
                top: parent.top
                margins: Globals.defaultMargin
            }
        }
        standardButtons: Dialog.Ok | Dialog.Cancel
        onAccepted: {
            item.deleteItem();
            page.closePage();
        }
    }

    ItemsSortFilterModel {
        id: undoneTodosModel
        sourceModel: ItemsModel {
            container: page.library.todos
        }
        todoList: page.item.uid
        onlyUndone: true
        searchString: filterBar.text
    }

    ItemsSortFilterModel {
        id: doneTodosModel
        sourceModel: ItemsModel {
            container: page.library.todos
        }
        todoList: page.item.uid
        onlyDone: true
        searchString: filterBar.text
    }

    TextInputBar {
        id: filterBar

        placeholderText: qsTr("Search term 1, search term 2, ...")
        symbol: Fonts.symbols.faTimes
        color: Colors.midlight
        itemCreator: false
        showWhenNonEmpty: true
        closeOnButtonClick: true
    }

    Rectangle {
        color: Colors.lightItemColor(item.color)
        anchors.fill: parent
    }

    ScrollView {
        id: scrollView

        anchors {
            left: parent.left
            right: parent.right
            bottom: parent.bottom
            top: filterBar.bottom
            topMargin: filterBar.shown ? (filterBar.contentHeight - filterBar.height) : 0
        }

        Column {
            width: scrollView.width
            spacing: Globals.defaultMargin

            ItemTitle {
                item: page.item
                width: parent.width
            }

            TodoListView {
                id: todos
                function openTodo(todo) {
                    page.openPage(todoPage, {todo: todo, library: page.library});
                }

                model: undoneTodosModel
                anchors {
                    left: parent.left
                    right: parent.right
                    margins: Globals.defaultMargin * 2
                }
                allowEntryCreation: true
                onTodoSelected: openTodo(todo)
                onAddEntry: {
                    var todo = page.item.addTodo();
                    todo.title = title;
                    if (openItem) {
                        todos.openTodo(todo);
                    }
                }
            }

            Symbol {
                anchors {
                    right: parent.right
                    rightMargin: Globals.defaultMargin
                }
                symbol: doneTodos.visible ? Fonts.symbols.faEye : Fonts.symbols.faEyeSlash
                visible: doneTodosModel.count > 0
                onClicked: doneTodos.visible = !doneTodos.visible
            }

            TodoListView {
                id: doneTodos
                function openTodo(todo) {
                    page.openPage(todoPage, {todo: todo, library: page.library});
                }

                visible: false
                model: doneTodosModel
                anchors {
                    left: parent.left
                    right: parent.right
                    margins: Globals.defaultMargin * 2
                }
                onTodoSelected: openTodo(todo)
            }

            StickyNote {
                id: note
                anchors {
                    left: parent.left
                    right: parent.right
                    margins: Globals.defaultMargin
                }
                title: qsTr("Notes")
                text: Globals.markdownToHtml(item.notes)
                onClicked: {
                    page.openPage(notesEditor, {"item": page.item});
                }
            }

            Attachments {
                item: page.item
                anchors {
                    left: parent.left
                    right: parent.right
                    margins: Globals.defaultMargin * 2
                }
            }

            TagsEditor {
                item: page.item
                library: page.library
                anchors {
                    left: parent.left
                    right: parent.right
                    margins: Globals.defaultMargin * 2
                }
            }

            Item {
                height: Globals.defaultMargin
                anchors {
                    left: parent.left
                    right: parent.right
                    margins: Globals.defaultMargin * 2
                }
            }

            Component {
                id: notesEditor

                RichTextEditor {
                    backgroundColor: Colors.lightItemColor(page.item.color)
                }
            }

            Component {
                id: todoPage
                TodoPage {
                    parentItemColor: page.item.color
                }
            }
        }
    }
}




