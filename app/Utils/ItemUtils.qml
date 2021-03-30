pragma Singleton

import QtQuick 2.0
import QtQuick.Controls 2.5
import QtQuick.Window 2.0

import OpenTodoList 1.0 as OTL

import "." as Utils
import "../Windows" as Windows

Item {
    id: root

    property Window window: null

    function renameItem(item) {
        if (d.renameItemDialog === null) {
            d.renameItemDialog = renameItemDialogComponent.createObject(window);
        }
        d.renameItemDialog.renameItem(item)
    }

    function deleteItem(item) {
        if (d.deleteItemDialog === null) {
            d.deleteItemDialog = deleteItemDialogComponent.createObject(window);
        }
        d.deleteItemDialog.deleteItem(item)
    }

    function selectDueToDate(item) {
        if (d.dateSelectionDialog === null) {
            d.dateSelectionDialog = dateSelectionDialogComponent.createObject(window);
        }
        d.dateSelectionDialog.item = item;
        d.dateSelectionDialog.selectedDate = item.dueTo;
        d.dateSelectionDialog.open();
    }

    function deleteCompletedItems(item) {
        if (d.deleteCompletedItemsDialog === null) {
            d.deleteCompletedItemsDialog = deleteCompletedItemsDialogComponent.createObject(window);
        }
        d.deleteCompletedItemsDialog.deleteCompletedItems(item);
    }

    function moveTodo(todo, library) {
        if (!library) {
            console.warn("Called ItemUtils.moveTodo() with null library");
            return;
        }

        if (d.moveTodoDialog === null) {
            d.moveTodoDialog = moveTodoDialogComponent.createObject(window);
        }
        d.moveTodoDialog.library = library;
        d.moveTodoDialog.todo = todo;
        d.moveTodoDialog.clear();
        d.moveTodoDialog.open();
    }

    function promoteTask(task, todoList, library) {
        if (!library) {
            console.warn("Called ItemUtils.promoteTask() with null library");
            return;
        }

        if (d.promoteTaskDialog === null) {
            d.promoteTaskDialog = promoteTaskDialogComponent.createObject(window);
        }
        d.promoteTaskDialog.library = library;
        d.promoteTaskDialog.task = task;
        d.promoteTaskDialog.initialTodoList = todoList;
        d.promoteTaskDialog.selectInitialTodoList = true;
        d.promoteTaskDialog.clear();
        d.promoteTaskDialog.open();
    }

    QtObject {
        id: d

        property Windows.RenameItemDialog renameItemDialog: null
        property Windows.DeleteItemDialog deleteItemDialog: null
        property Windows.DeleteCompletedItemsDialog deleteCompletedItemsDialog: null
        property Windows.DateSelectionDialog dateSelectionDialog: null
        property Windows.SelectTodoListDialog moveTodoDialog: null
        property Windows.SelectTodoListDialog promoteTaskDialog: null

    }

    Component {
        id: renameItemDialogComponent

        Windows.RenameItemDialog {}
    }

    Component {
        id: deleteItemDialogComponent

        Windows.DeleteItemDialog {}
    }

    Component {
        id: deleteCompletedItemsDialogComponent

        Windows.DeleteCompletedItemsDialog {}
    }

    Component {
        id: dateSelectionDialogComponent

        Windows.DateSelectionDialog {
            property var item: null

            onAccepted: if (item !== null) {
                item.dueTo = selectedDate;
            }
        }
    }

    Component {
        id: moveTodoDialogComponent

        Windows.SelectTodoListDialog {
            property  OTL.Todo todo: null

            title: qsTr("Move Todo Into...")
            onAccepted: {
                if (todo && selectedTodoList) {
                    OTL.Application.moveTodo(todo, selectedTodoList);
                }
            }
        }
    }

    Component {
        id: promoteTaskDialogComponent

        Windows.SelectTodoListDialog {
            property  OTL.Task task: null

            title: qsTr("Convert Task to Todo and Move Into...")
            onAccepted: {
                if (task && selectedTodoList) {
                    OTL.Application.promoteTask(task, selectedTodoList);
                }
            }
        }
    }

}
