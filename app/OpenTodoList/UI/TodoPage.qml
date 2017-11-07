import QtQuick 2.5
import QtQuick.Controls 2.2

import OpenTodoList 1.0
import OpenTodoList.UI 1.0

Page {
    id: page

    property Library library: null
    property Todo todo: Todo {}

    signal closePage()
    signal openPage(var component, var properties)


    function newTask() {
        tasks.focusNewItemInput();
    }

    function cancel() {
    }

    function deleteItem() {
        confirmDeleteDialog.open();
    }

    function find() {
        filterBar.edit.forceActiveFocus()
    }

    CenteredDialog {
        id: confirmDeleteDialog
        title: qsTr("Delete Todo?")
        Text {
            text: qsTr("Are you sure you want to delete the todo <strong>%1</strong>? This action " +
                       "cannot be undone.").arg(todo.title)
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
            todo.deleteItem();
            page.closePage();
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
                item: page.todo
                width: parent.width
            }

            TaskListView {
                id: tasks
                model: ItemsSortFilterModel {
                    sourceModel: ItemsModel {
                        id: sourceModel
                        container: page.library.tasks
                    }
                    todo: page.todo.uid
                    searchString: filterBar.text
                }

                anchors {
                    left: parent.left
                    right: parent.right
                    margins: Globals.defaultMargin * 2
                }
                allowNewEntryCreation: true
                onAddEntry: {
                    var task = todo.addTask();
                    task.title = title;
                }
            }

            StickyNote {
                id: note
                anchors {
                    left: parent.left
                    right: parent.right
                    margins: Globals.defaultMargin
                }
                title: qsTr("Notes")
                text: todo.notes
                backgroundColor: Colors.itemYellow
                onClicked: {
                    page.openPage(notesEditor, {item: page.todo});
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
        }
    }
}




