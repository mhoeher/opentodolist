import QtQuick 2.5
import QtCore

import OpenTodoList 1.0 as OTL

import "../Components"
import "../Components/Tooltips" as Tooltips
import "../Controls" as C
import "../Fonts"
import "../Windows"
import "../Widgets"
import "../Utils"
import "../Actions" as Actions

ItemPage {
    id: page

    property OTL.Todo item: OTL.Todo {}
    property OTL.Library library: null
    property OTL.TodoList todoList: null

    signal closePage
    signal openPage(var component, var properties)

    readonly property alias editingNotes: d.editingNotes

    function finishEditingNotes() {
        todosWidget.headerItem.item.itemNotesEditor.finishEditing()
    }

    function deleteItem() {
        confirmDeleteDialog.deleteItem(item)
    }

    function deleteCompletedItems() {
        C.ApplicationWindow.window.itemUtils.deleteCompletedItems(item)
    }

    function renameItem() {
        renameItemDialog.renameItem(item)
    }

    function moveItem() {
        moveTodoAction.trigger()
    }

    function copyItem() {
        copyTodoAction.trigger()
    }

    function copyLinkToPage() {
        let url = shareUtils.createDeepLink(page.item)
        OTL.Application.copyToClipboard(url.toString())
    }

    function setProgress() {
        setManualProgressAction.trigger()
    }

    function setDueDate() {
        dueDateSelectionDialog.selectedDate = item.dueTo
        dueDateSelectionDialog.open()
    }

    function find() {
        filterBar.edit.forceActiveFocus()
    }

    function attach() {
        d.attach()
    }


    /*
      Attaches the list of files to the todo list.
      */
    function attachFiles(fileUrls) {
        d.attachFiles(fileUrls)
    }

    property var goBack: {
        if (editingNotes) {
            return function () {
                todosWidget.headerItem.item.itemNotesEditor.finishEditing()
            }
        } else {
            return undefined
        }
    }

    function openInNewWindow() {
        openStackViewWindow(restoreUrl, {
                                "item": OTL.Application.cloneItem(page.item),
                                "todoList": OTL.Application.cloneItem(
                                                page.todoList),
                                "library": OTL.Application.cloneLibrary(
                                               page.library)
                            })
    }

    title: Markdown.markdownToPlainText(item.title)
    topLevelItem: todoList

    savePage: function () {
        return {
            "library": OTL.Application.uuidToString(page.library.uid),
            "todoList": OTL.Application.uuidToString(page.todoList.uid),
            "todo": OTL.Application.uuidToString(page.item.uid)
        }
    }

    restorePage: function (state) {
        d.restoreLibraryUid = OTL.Application.uuidFromString(state.library)
        d.restoreTodoListUid = OTL.Application.uuidFromString(state.todoList)
        d.restoreTodoUid = OTL.Application.uuidFromString(state.todo)
        d.loadLibraryTransactionId = OTL.Application.loadLibrary(
                    d.restoreLibraryUid)
        d.loadTodoListTransactionId = OTL.Application.loadItem(
                    d.restoreTodoListUid)
        d.loadTodoTransactionId = OTL.Application.loadItem(d.restoreTodoUid)
    }

    restoreUrl: Qt.resolvedUrl("./TodoPage.qml")

    QtObject {
        id: d

        property bool editingNotes: false

        property var restoreLibraryUid
        property var restoreTodoListUid
        property var restoreTodoUid
        property var loadLibraryTransactionId
        property var loadTodoListTransactionId
        property var loadTodoTransactionId

        signal attach
        signal attachFiles(var fileUrls)
    }

    Actions.MoveTodo {
        id: moveTodoAction
        library: page.library
        item: page.item
        itemUtils: page.C.ApplicationWindow.window.itemUtils
    }

    Actions.SetManualProgressAction {
        id: setManualProgressAction
        item: page.item
    }

    DeleteItemDialog {
        id: confirmDeleteDialog
        onAccepted: {
            page.closePage()
        }
    }

    DateSelectionDialog {
        id: dueDateSelectionDialog
        onAccepted: page.item.dueTo = selectedDate
    }

    RenameItemDialog {
        id: renameItemDialog
    }

    OTL.ItemsSortFilterModel {
        id: tasks
        groupDone: settings.groupDone
        sourceModel: OTL.ItemsModel {
            cache: OTL.Application.cache
            parentItem: page.item.uid
            searchString: filterBar.text
            onlyUndone: !settings.showUndone
        }
    }

    TextInputBar {
        id: filterBar
        placeholderText: qsTr("Search term 1, search term 2, ...")
        symbol: Icons.mdiClose
        showWhenNonEmpty: true
        closeOnButtonClick: true
    }

    ItemScrollView {
        id: scrollView
        anchors {
            left: parent.left
            right: parent.right
            top: filterBar.bottom
            bottom: parent.bottom
        }
        item: page.todoList
        C.ScrollBar.vertical.policy: (todosWidget.headerItem.item.itemNotesEditor
                                      || {}).editing ? C.ScrollBar.AlwaysOn : C.ScrollBar.AsNeeded
        C.ScrollBar.vertical.interactive: true

        TodosWidget {
            id: todosWidget

            width: scrollView.contentItem.width
            implicitWidth: childrenRect.width
            implicitHeight: childrenRect.height
            itemsModel: tasks
            title: qsTr("Tasks")
            library: page.library
            parentItem: page.todoList
            headerItemVisible: true
            symbol: settings.showUndone ? Icons.mdiVisibility : Icons.mdiVisibilityOff
            allowCreatingNewItems: true
            newItemPlaceholderText: qsTr("Add new task...")
            onHeaderButtonClicked: tasksVisibilityMenu.open()
            onCreateNewItem: {
                var properties = {
                    "title": title
                }
                var task = OTL.Application.addTask(page.library, page.item,
                                                   properties)
            }

            headerComponent: Column {
                id: column

                property alias itemNotesEditor: itemNotesEditor

                width: parent.width
                spacing: AppSettings.smallSpace

                ItemPageHeader {
                    item: page.item
                }

                ItemProgressEditor {
                    item: page.item
                    width: parent.width
                    visible: page.item.progress >= 0
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

                    Binding {
                        target: d
                        property: "editingNotes"
                        value: itemNotesEditor.editing
                    }
                }
            }

            footerComponent: Attachments {
                id: attachments
                item: page.item
                width: parent.width

                Connections {
                    target: d
                    function onAttach() {
                        attach()
                    }
                    function onAttachFiles(fileUrls) {
                        attachFiles(fileUrls)
                    }
                }
            }
        }
    }

    PullToRefreshOverlay {
        anchors.fill: scrollView
        refreshEnabled: page.library.hasSynchronizer
        flickable: todosWidget
        onRefresh: OTL.Application.syncLibrary(page.library)
    }

    AutoScrollOverlay {
        anchors.fill: parent
        flickable: todosWidget
    }

    Settings {
        id: settings
        category: "TodoPage"

        property bool showUndone: false
        property bool groupDone: false
    }

    C.Menu {
        id: tasksVisibilityMenu
        parent: todosWidget.headerIcon
        modal: true

        C.MenuItem {
            text: qsTr("Show Completed")
            checked: settings.showUndone
            checkable: true
            onClicked: settings.showUndone = !settings.showUndone
        }

        C.MenuItem {
            text: qsTr("Show At The End")
            checked: settings.groupDone
            checkable: true
            visible: settings.showUndone
            height: visible ? implicitHeight : 0
            onClicked: settings.groupDone = !settings.groupDone
        }
    }

    Actions.CopyTodo {
        id: copyTodoAction
        item: page.item
        itemUtils: page.C.ApplicationWindow.window.itemUtils
    }

    Connections {
        target: OTL.Application

        function onLibraryLoaded(uid, data, transactionId) {
            if (uid === d.restoreLibraryUid
                    && transactionId === d.loadLibraryTransactionId) {
                page.library = OTL.Application.libraryFromData(data)
            }
        }

        function onItemLoaded(uid, data, parents, library, transactionId) {
            if (uid === d.restoreTodoListUid
                    && transactionId === d.loadTodoListTransactionId) {
                page.todoList = OTL.Application.itemFromData(data)
            } else if (uid === d.restoreTodoUid
                       && transactionId === d.loadTodoTransactionId) {
                page.item = OTL.Application.itemFromData(data)
            }
        }
    }

    Tooltips.AllSubtasksDone {
        item: page.item
    }
}
