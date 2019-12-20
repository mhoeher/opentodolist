import QtQuick 2.5
import QtQuick.Controls 2.12

import OpenTodoList 1.0 as OTL

import "../Components"
import "../Windows"
import "../Widgets"
import "../Utils"

ItemPage {
    id: page
    
    property var library: null
    property OTL.Note item: OTL.Note {}

    signal closePage()
    signal openPage(var component, var properties)

    property var goBack: itemNotesEditor.editing ? function() {
        itemNotesEditor.finishEditing();
    } : undefined
    
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

    function setDueDate() {
        dueDateSelectionDialog.selectedDate = item.dueTo;
        dueDateSelectionDialog.open();
    }

    title: titleText.text
    topLevelItem: item

    MarkdownConverter {
        id: titleText
        markdown: item.title
        strip: true
    }
    
    DeleteItemDialog {
        id: confirmDeleteDialog
        onAccepted: page.closePage()
    }

    DateSelectionDialog {
        id: dueDateSelectionDialog
        onAccepted: page.item.dueTo = selectedDate
    }

    RenameItemDialog {
        id: renameItemDialog
    }

    ItemScrollView {
        id: scrollView

        anchors.fill: parent
        item: page.item
        padding: 10

        Column {
            width: scrollView.contentItem.width
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
                id: itemNotesEditor
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

