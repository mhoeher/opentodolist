import QtQuick 2.5
import QtQuick.Controls 2.2

import OpenTodoList 1.0
import OpenTodoList.UI 1.0

Item {
    id: page

    property Library library: null
    property Todo todo: Todo {}
    property StackView stack: null


    function newTask() {
        tasks.focusNewItemInput();
    }

    function cancel() {
    }

    function deleteItem() {
        confirmDeleteDialog.open();
    }

    Dialog {
        id: confirmDeleteDialog
        title: qsTr("Delete Todo?")
        Text {
            text: qsTr("Are you sure you want to delete the todo <strong>%1</strong>? This action " +
                       "cannot be undone.").arg(todo.title)
            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
        }
        standardButtons: Dialog.Ok | Dialog.Cancel
        onAccepted: {
            todo.deleteItem();
            stack.pop();
        }
    }

    RenameItemDialog {
        id: renameItemDialog
    }

    ScrollView {
        id: scrollView

        anchors.fill: parent

        Column {
            width: scrollView.width
            spacing: Globals.defaultMargin

            MouseArea {
                onClicked: renameItemDialog.renameItem(todo)
                height: childrenRect.height
                width: parent.width
                cursorShape: Qt.PointingHandCursor

                Label {
                    text: todo.title
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

            TaskListView {
                id: tasks
                model: ItemsSortFilterModel {
                    sourceModel: ItemsModel {
                        id: sourceModel
                        container: page.library.tasks
                    }
                    filterFunction: function(row) {
                        var idx = sourceModel.index(row, 0);
                        var task = sourceModel.data(idx, ItemsModel.ItemRole);
                        return task.todoUid === page.todo.uid;
                    }
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
                    var page = stack.push(notesEditor, { text: todo.notes });
                    page.onTextChanged.connect(function() { todo.notes = page.text; });
                    todo.onReloaded.connect(function() { page.text = todo.notes; });
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
                    Component.onCompleted: forceActiveFocus()
                }
            }
        }
    }
}




