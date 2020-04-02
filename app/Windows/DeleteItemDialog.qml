import QtQuick 2.0
import QtQuick.Controls 2.12

import OpenTodoList 1.0 as OTL

import "../Components"
import "../Utils"


CenteredDialog {
    id: dialog

    function deleteItem(item) {
        d.item = item;
        dialog.open();
    }

    standardButtons: Dialog.Ok | Dialog.Cancel
    title: qsTr("Delete Item?")
    width: idealDialogWidth

    onAccepted: {
        OTL.Application.deleteItem(d.item);
    }

    QtObject {
        id: d

        property OTL.Item item
        property OTL.Library library

        readonly property string deleteImage: qsTr("Do you really want " +
                                                   "to delete the image " +
                                                   "<strong>%1</strong>? This " +
                                                   "cannot be undone.")
        readonly property string deleteTodoList: qsTr("Do you really want " +
                                                   "to delete the todo list " +
                                                   "<strong>%1</strong>? This " +
                                                   "cannot be undone.")
        readonly property string deleteTodo: qsTr("Do you really want " +
                                                   "to delete the todo " +
                                                   "<strong>%1</strong>? This " +
                                                   "cannot be undone.")
        readonly property string deleteTask: qsTr("Do you really want " +
                                                   "to delete the task " +
                                                   "<strong>%1</strong>? This " +
                                                   "cannot be undone.")
        readonly property string deleteNote: qsTr("Do you really want " +
                                                   "to delete the note " +
                                                   "<strong>%1</strong>? This " +
                                                   "cannot be undone.")
        readonly property string deleteNotePage: qsTr("Do you really want " +
                                                     "to delete the page " +
                                                     "<strong>%1</strong>? This " +
                                                     "cannot be undone.")
    }

    Label {
        width: dialog.availableWidth
        text: {
            if (d.item) {
                var msg;
                switch (d.item.itemType) {
                case "Image":
                    msg = d.deleteImage;
                    break;
                case "TodoList":
                    msg = d.deleteTodoList;
                    break;
                case "Todo":
                    msg = d.deleteTodo;
                    break;
                case "Task":
                    msg = d.deleteTask;
                    break;
                case "Note":
                    msg = d.deleteNote;
                    break;
                case "NotePage":
                    msg = d.deleteNotePage;
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
