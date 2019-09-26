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
    property OTL.Image item: OTL.Image {}

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

    RenameItemDialog {
        id: renameItemDialog
    }

    DateSelectionDialog {
        id: dueDateSelectionDialog
        onAccepted: page.item.dueTo = selectedDate
    }

    ItemScrollView {
        id: scrollView

        anchors.fill: parent
        item: page.item
        padding: 10

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

            Frame {
                width: parent.width
                height: image.height + padding * 2

                Image {
                    id: image

                    source: item.imageUrl
                    width: parent.width
                    height: parent.width * sourceSize.height / sourceSize.width
                }

                MouseArea {
                    anchors.fill: image
                    onClicked: Qt.openUrlExternally(item.imageUrl)
                }
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

