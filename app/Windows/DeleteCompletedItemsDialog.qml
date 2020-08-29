import QtQuick 2.0
import QtQuick.Controls 2.12

import OpenTodoList 1.0 as OTL

import "../Components"
import "../Utils"


CenteredDialog {
    id: dialog

    function deleteCompletedItems(item) {
        d.item = item;
        dialog.open();
    }

    standardButtons: Dialog.Ok | Dialog.Cancel
    title: qsTr("Delete Completed Items?")
    width: idealDialogWidth

    onAccepted: {
        switch (d.item.itemType) {
        case "TodoList":
            OTL.Application.deleteDoneTodos(d.item);
            break;
        case "Todo":
            OTL.Application.deleteDoneTasks(d.item);
            break;
        default:
            console.error("Unhandled item type ", d.item.itemType, " for item ", d.item);
            break;
        }
    }

    QtObject {
        id: d

        property OTL.Item item
        property OTL.Library library

        readonly property string deleteTodoListItems: qsTr(
                                                          "Do you really want to delete all done " +
                                                          "todos in the todo list " +
                                                          "<strong>%1</strong>? This cannot be " +
                                                          "undone."
                                                          )
        readonly property string deleteTodoItems: qsTr(
                                                      "Do you really want to delete all done " +
                                                      "tasks in the todo " +
                                                      "<strong>%1</strong>? This cannot be " +
                                                      "undone."
                                                      )
    }

    Label {
        width: dialog.availableWidth
        text: {
            if (d.item) {
                var msg;
                switch (d.item.itemType) {
                case "TodoList":
                    msg = d.deleteTodoListItems;
                    break;
                case "Todo":
                    msg = d.deleteTodoItems;
                    break;
                default:
                    console.debug("Warning: Unhandled item type " +
                                  d.item.itemType);
                    return;
                }
                return msg.arg(Markdown.markdownToPlainText(d.item.title));
            }
            return "";
        }
    }
}
