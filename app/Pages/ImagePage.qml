import QtQuick 2.5
import QtQuick.Controls 2.12

import OpenTodoList 1.0 as OTL

import "../Components"
import "../Windows"
import "../Widgets"
import "../Utils"
import "../Menues"

ItemPage {
    id: page

    property var library: null
    property OTL.ImageItem item: OTL.ImageItem {}

    signal closePage()
    signal openPage(var component, var properties)

    property var goBack: itemNotesEditor.editing ? function() {
        itemNotesEditor.finishEditing();
    } : undefined
    property alias pageActions: libraryActions.actions

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

    title: Markdown.markdownToPlainText(item.title)
    topLevelItem: item

    LibraryPageActions {
        id: libraryActions

        library: page.library
        onOpenPage: page.openPage(component, properties)
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
        padding: AppSettings.mediumSpace
        ScrollBar.vertical.policy: itemNotesEditor.editing ? ScrollBar.AlwaysOn : ScrollBar.AsNeeded
        ScrollBar.vertical.interactive: true

        Flickable {
            id: flickable

            width: scrollView.width
            contentWidth: width
            contentHeight: column.height

            Column {
                id: column
                width: flickable.width
                spacing: AppSettings.largeSpace

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
                        onClicked: OTL.Application.openUrl(item.imageUrl)
                    }
                }

                ItemDueDateEditor {
                    id: itemDueDateEditor
                    item: page.item
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

    PullToRefreshOverlay {
        anchors.fill: scrollView
        refreshEnabled: page.library.hasSynchronizer
        flickable: flickable
        onRefresh: OTL.Application.syncLibrary(page.library)
    }

}

