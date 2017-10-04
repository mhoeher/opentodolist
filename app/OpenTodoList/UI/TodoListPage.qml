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
        Text {
            text: qsTr("Are you sure you want to delete the todo list <strong>%1</strong>? This action " +
                       "cannot be undone.").arg(item.title)
            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
            width: 300
        }
        standardButtons: Dialog.Ok | Dialog.Cancel
        onAccepted: {
            item.deleteItem();
            page.closePage();
        }
    }

    RenameItemDialog {
        id: renameItemDialog
    }

    ItemsSortFilterModel {
        id: todosModel
        sourceModel: ItemsModel {
            id: allTodosModel
            container: page.library.todos
        }
        filterFunction: function(row) {
            var idx = allTodosModel.index(row, 0);
            var todo = allTodosModel.data(idx, ItemsModel.ItemRole);
            return todo.todoListUid === page.item.uid;
        }
    }

    ItemsSortFilterModel {
        id: undoneTodosModel
        sourceModel: todosModel
        filterFunction: function(i) {
            var todo = sourceModel.data(sourceModel.index(i, 0), ItemsModel.ItemRole);
            return !todo.done;
        }
    }

    ItemsSortFilterModel {
        id: doneTodosModel
        sourceModel: todosModel
        filterFunction: function(i) {
            var todo = sourceModel.data(sourceModel.index(i, 0), ItemsModel.ObjectRole);
            return todo.done;
        }
    }

    TextInputBar {
        id: filterBar

        placeholderText: qsTr("Search term 1, search term 2, ...")
        symbol: Fonts.symbols.faTimes
        color: Colors.secondary2
        itemCreator: false
        showWhenNonEmpty: true
        closeOnButtonClick: true

        onTextChanged: {
            function match(todo) {
                return Logic.itemMatchesFilterWithDefault(todo, text, true);
            }
            undoneTodosModel.filterFunction = function(i) {
                var todo = undoneTodosModel.sourceModel.data(
                            undoneTodosModel.sourceModel.index(i, 0), ItemsModel.ItemRole);
                return !todo.done && match(todo);
            }
            doneTodosModel.filterFunction = function(i) {
                var todo = doneTodosModel.sourceModel.data(
                            doneTodosModel.sourceModel.index(i, 0), ItemsModel.ItemRole);
                return todo.done && match(todo);
            }
        }
    }

    Rectangle {
        color: Qt.lighter(Colors.itemColor(item.color), 1.1)
        opacity: 0.3
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

            MouseArea {
                onClicked: renameItemDialog.renameItem(item)
                height: childrenRect.height
                width: parent.width
                cursorShape: Qt.PointingHandCursor

                Label {
                    text: item.title
                    anchors {
                        left: parent.left
                        right: parent.right
                        margins: Globals.defaultMargin
                    }
                    font {
                        bold: true
                        pixelSize: Globals.fontPixelSize * 2
                    }
                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                }
            }

            TodoListView {
                id: todos
                function openTodo(todo) {
                    page.openPage(todoPage, {item: page.todo});
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
                    page.openPage(todoPage, {item: todo});
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
                text: item.notes
                backgroundColor: item.color === TopLevelItem.White ? Colors.noteBackground : Colors.itemWhite
                onClicked: {
                    page.openPage(notesEditor, {"item": page.item});
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

                RichTextEditor {}
            }

            Component {
                id: todoPage
                TodoPage {}
            }
        }
    }
}



