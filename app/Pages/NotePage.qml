import QtQuick 2.5

import OpenTodoList 1.0 as OTL

import "../Components"
import "../Windows"
import "../Widgets"
import "../Utils"

Page {
    id: page
    
    property OTL.Note item: OTL.Note {}
    property var library: null

    signal closePage()
    signal openPage(var component, var properties)
    
    function deleteItem() {
        confirmDeleteDialog.deleteItem(item);
    }

    function renameItem() {
        renameItemDialog.renameItem(item);
    }

    title: titleText.text

    MarkdownConverter {
        id: titleText
        markdown: item.title
        strip: true
    }
    
    DeleteItemDialog {
        id: confirmDeleteDialog
        onAccepted: page.closePage()
    }

    RenameItemDialog {
        id: renameItemDialog
    }

    OTL.ItemsModel {
        id: todos
        container: page.library.todos
    }

    OTL.ItemsSortFilterModel {
        id: undoneTodos

        sourceModel: todos
        todoList: page.item.uid
        onlyUndone: true
    }

    OTL.ItemsSortFilterModel {
        id: doneTodos

        sourceModel: todos
        todoList: page.item.uid
        onlyDone: true
    }

    Pane {
        id: background

        backgroundColor: Colors.color(Colors.itemColor(item), Colors.shade50)
        anchors.fill: parent

        ScrollView {
            id: scrollView

            anchors.fill: parent

            Column {
                width: scrollView.width

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

                TagsEditor {
                    item: page.item
                    library: page.library
                    width: parent.width
                }
            }
        }
    }
}

