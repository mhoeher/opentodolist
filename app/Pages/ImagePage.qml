import QtQuick 2.5

import OpenTodoList 1.0 as OTL

import "../Components"
import "../Windows"
import "../Widgets"
import "../Utils"
import "../Menues"
import "../Actions" as Actions
import "../Controls" as C

ItemPage {
    id: page

    property var library: null
    property OTL.ImageItem item: OTL.ImageItem {}

    signal closePage
    signal openPage(var component, var properties)

    property var goBack: {
        if (itemNotesEditor.editing) {
            return () => itemNotesEditor.finishEditing()
        }
    }

    function deleteItem() {
        confirmDeleteDialog.deleteItem(item)
    }

    function renameItem() {
        renameItemDialog.renameItem(item)
    }

    function copyItem() {
        copyTopLevelItemAction.trigger()
    }

    function addTag() {
        tagsEditor.addTag()
    }

    function attach() {
        attachments.attach()
    }


    /*
      Attach the files to the image.
      */
    function attachFiles(fileUrls) {
        attachments.attachFiles(fileUrls)
    }

    function setDueDate() {
        dueDateSelectionDialog.selectedDate = item.dueTo
        dueDateSelectionDialog.open()
    }

    title: Markdown.markdownToPlainText(item.title)
    topLevelItem: item

    savePage: function () {
        return {
            "library": OTL.Application.uuidToString(page.library.uid),
            "image": OTL.Application.uuidToString(page.item.uid)
        }
    }
    restorePage: function (state) {
        d.restoreImageUid = OTL.Application.uuidFromString(state.image)
        d.restoreLibraryUid = OTL.Application.uuidFromString(state.library)
        OTL.Application.loadLibrary(d.restoreLibraryUid)
        OTL.Application.loadItem(d.restoreImageUid)
    }
    restoreUrl: Qt.resolvedUrl("./ImagePage.qml")

    QtObject {
        id: d

        property var restoreLibraryUid
        property var restoreImageUid
    }

    OTL.ShareUtils {
        id: shareUtils
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
        C.ScrollBar.vertical.policy: itemNotesEditor.editing ? C.ScrollBar.AlwaysOn : C.ScrollBar.AsNeeded
        C.ScrollBar.vertical.interactive: true

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

                C.Frame {
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
                        onClicked: shareUtils.openFile(item.imageUrl)
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

    Actions.CopyTopLevelItem {
        id: copyTopLevelItemAction
        item: page.item
    }

    Connections {
        target: OTL.Application

        function onLibraryLoaded(uid, data) {
            if (uid === d.restoreLibraryUid) {
                page.library = OTL.Application.libraryFromData(data)
            }
        }

        function onItemLoaded(uid, data) {
            if (uid === d.restoreImageUid) {
                page.item = OTL.Application.itemFromData(data)
            }
        }
    }
}
