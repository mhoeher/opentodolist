import QtQuick 2.5
import QtQuick.Layouts 1.0

import OpenTodoList 1.0 as OTL

import "../Components"
import "../Windows"
import "../Widgets"
import "../Utils"
import "../Fonts"
import "../Menues"
import "../Actions" as Actions
import "../Controls" as C

ItemPage {
    id: page

    property var library: null
    property OTL.Note item: OTL.Note {}

    signal closePage
    signal openPage(var component, var properties)

    property var goBack: itemNotesEditor.editing ? function () {
        itemNotesEditor.finishEditing()
    } : undefined
    property alias pageActions: libraryActions.actions

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
      Attaches the list of files to the todo list.
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

    LibraryPageActions {
        id: libraryActions

        library: page.library
        onOpenPage: page.openPage(component, properties)
    }

    DeleteItemDialog {
        id: confirmDeleteDialog
        onAccepted: page.closePage()
    }

    DeleteItemDialog {
        id: confirmDeletePageDialog
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
        padding: AppSettings.mediumSpace
        C.ScrollBar.vertical.policy: itemNotesEditor.editing ? C.ScrollBar.AlwaysOn : C.ScrollBar.AsNeeded
        C.ScrollBar.vertical.interactive: true

        Flickable {
            id: flickable
            width: scrollView.availableWidth
            height: scrollView.availableHeight
            contentWidth: column.width
            contentHeight: column.height

            Column {
                id: column

                width: scrollView.availableWidth
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

                ItemDueDateEditor {
                    id: itemDueDateEditor
                    item: page.item
                    width: parent.width
                }

                RowLayout {
                    width: parent.width

                    C.TabBar {
                        id: pageTabBar
                        Layout.fillWidth: true
                        clip: true

                        C.TabButton {
                            id: mainPageTabButton

                            text: qsTr("Main Page")
                            width: implicitWidth
                        }

                        Repeater {
                            model: OTL.ItemsSortFilterModel {
                                id: pagesModel

                                sortRole: OTL.ItemsModel.WeightRole
                                sourceModel: OTL.ItemsModel {
                                    cache: OTL.Application.cache
                                    parentItem: page.item.uid
                                    onCountChanged: {
                                        var lastPage = itemNotesEditor.lastPageCreated
                                        if (lastPage != null) {
                                            for (var i = 0; i < count; ++i) {
                                                var idx = index(i, 0)
                                                var modelPage = data(
                                                            idx,
                                                            OTL.ItemsModel.ItemRole)
                                                if (modelPage.uid === lastPage.uid) {
                                                    pageTabBar.setCurrentIndex(
                                                                i + 1)
                                                    itemNotesEditor.lastPageCreated = null
                                                    break
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                            delegate: C.TabButton {
                                text: title
                                width: implicitWidth
                            }
                        }
                    }
                    C.ToolButton {
                        symbol: Icons.mdiKeyboardArrowDown
                        visible: pageTabBar.contentWidth > pageTabBar.width

                        onClicked: pageMenu.popup()

                        C.Menu {
                            id: pageMenu

                            modal: true

                            C.MenuItem {
                                text: mainPageTabButton.text
                                onClicked: pageTabBar.setCurrentIndex(0)
                            }

                            Repeater {
                                model: pagesModel
                                delegate: C.MenuItem {
                                    text: title
                                    onTriggered: pageTabBar.setCurrentIndex(
                                                     index + 1)
                                }
                            }
                        }
                    }
                    C.ToolButton {
                        symbol: Icons.mdiNoteAdd
                        onClicked: {
                            var args = {
                                "title": qsTr("New Page")
                            }

                            var notePage = OTL.Application.addNotePage(
                                        page.library, page.item, args)
                            itemNotesEditor.lastPageCreated = notePage
                        }
                    }
                }

                ItemNotesEditor {
                    id: itemNotesEditor

                    property OTL.ComplexItem editingItem: {
                        if (pageTabBar.currentIndex === 0) {
                            return page.item
                        } else {
                            return pagesModel.data(
                                        pagesModel.index(
                                            pageTabBar.currentIndex - 1, 0),
                                        OTL.ItemsModel.ItemRole)
                        }
                    }
                    property OTL.NotePageItem lastPageCreated: null

                    width: parent.width
                    extraButton.visible: pageTabBar.currentIndex > 0
                    extraButton.onClicked: {
                        renameItemDialog.renameItem(editingItem)
                    }
                    extraButton2.visible: pageTabBar.currentIndex > 0
                    extraButton2.onClicked: {
                        confirmDeletePageDialog.deleteItem(editingItem)
                    }

                    onEditingItemChanged: {
                        // Indirection: Make sure we save before changing item to be edited.
                        finishEditing()
                        item = editingItem
                    }
                    Component.onCompleted: item = editingItem
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
}
