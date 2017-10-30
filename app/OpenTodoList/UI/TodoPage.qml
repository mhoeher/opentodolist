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

    CenteredDialog {
        id: confirmDeleteDialog
        title: qsTr("Delete Todo?")
        Text {
            text: qsTr("Are you sure you want to delete the todo <strong>%1</strong>? This action " +
                       "cannot be undone.").arg(todo.title)
            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
            width: 300
        }
        standardButtons: Dialog.Ok | Dialog.Cancel
        onAccepted: {
            todo.deleteItem();
            page.closePage();
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
                    todo: page.todo.uid
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




