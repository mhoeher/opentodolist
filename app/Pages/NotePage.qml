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

    function addTag() {
        tagsEditor.addTag();
    }

    function attach() {
        attachments.attach();
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

    Pane {
        anchors.fill: parent
        backgroundColor: Colors.color(Colors.itemColor(item), Colors.shade50)
    }

    ScrollView {
        id: scrollView

        anchors.fill: parent

        Pane {
            id: background

            backgroundColor: Colors.color(Colors.itemColor(item), Colors.shade50)
            width: scrollView.width

            Column {
                width: parent.width
                spacing: 20

                ItemPageHeader {
                    item: page.item
                }

                TagsEditor {
                    id: tagsEditor
                    item: page.item
                    library: page.library
                    width: parent.width
                }

                ItemNotesEditor {
                    item: page.item
                    width: parent.width
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

