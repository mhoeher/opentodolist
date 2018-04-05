import QtQuick 2.5

import OpenTodoList 1.0 as OTL

import "../Components"
import "../Windows"
import "../Widgets"
import "../Utils"

Page {
    id: page

    property OTL.Image item: OTL.Image {}
    property var library: null

    signal closePage()
    signal openPage(var component, var properties)

    function deleteItem() {
        confirmDeleteDialog.deleteItem(item);
    }

    function renameItem() {
        renameItemDialog.renameItem(item);
    }

    title: OTL.Application.htmlToPlainText(Markdown.format(item.title))

    DeleteItemDialog {
        id: confirmDeleteDialog
        onAccepted: page.closePage()
    }

    RenameItemDialog {
        id: renameItemDialog
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

                Frame {
                    width: parent.width
                    height: image.height + padding * 2

                    Image {
                        id: image

                        source: item.imageUrl
                        width: parent.width
                        height: parent.width * sourceSize.height / sourceSize.width
                    }
                }

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

