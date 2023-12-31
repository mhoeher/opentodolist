import QtQuick 2.5
import QtQuick.Layouts 1.12
import QtQuick.Controls.Material 2.0
import QtCore

import OpenTodoList as OTL

import "../Components"
import "../Components/Tooltips" as Tooltips
import "../Controls" as C
import "../Fonts"
import "../Windows"
import "../Widgets"
import "../Utils"
import "../Menues"
import "../Actions" as Actions

ItemPage {
    id: page

    property OTL.TodoList item: null
    property OTL.Library library: null
    signal closePage

    signal openPage(var component, var properties)

    function deleteItem() {
        confirmDeleteDialog.deleteItem(item)
    }

    function deleteCompletedItems() {
        C.ApplicationWindow.window.itemUtils.deleteCompletedItems(item)
    }

    function renameItem() {
        renameItemDialog.renameItem(item)
    }

    function copyItem() {
        copyTopLevelItemAction.trigger()
    }

    function copyLinkToPage() {
        let url = shareUtils.createDeepLink(page.item)
        OTL.Application.copyToClipboard(url.toString())
    }

    function find() {
        filterBar.edit.forceActiveFocus()
    }

    property var goBack: {
        if (d.editingNotes) {
            return () => todosWidget.headerItem.item.itemNotesEditor.finishEditing()
        }
    }

    function addTag() {
        d.openTagsEditor()
    }

    function attach() {
        d.attach()
    }


    /**
     * @brief Attaches the list of files to the todo list.
     */
    function attachFiles(fileUrls) {
        d.attachFiles(fileUrls)
    }

    function setDueDate() {
        dueDateSelectionDialog.selectedDate = item.dueTo
        dueDateSelectionDialog.open()
    }

    function openInNewWindow() {
        openStackViewWindow(restoreUrl, {
                                "item": OTL.Application.cloneItem(page.item),
                                "library": OTL.Application.cloneLibrary(
                                               page.library)
                            })
    }

    savePage: function () {
        return {
            "library": OTL.Application.uuidToString(page.library.uid),
            "todoList": OTL.Application.uuidToString(page.item.uid)
        }
    }

    restorePage: function (state) {
        d.restoreLibraryUid = OTL.Application.uuidFromString(state.library)
        d.restoreTodoListUid = OTL.Application.uuidFromString(state.todoList)
        d.loadLibraryTransactionId = OTL.Application.loadLibrary(
                    d.restoreLibraryUid)
        d.loadTodoListTransactionId = OTL.Application.loadItem(
                    d.restoreTodoListUid)
    }

    restoreUrl: Qt.resolvedUrl("./TodoListPage.qml")

    title: Markdown.markdownToPlainText(item.title)
    topLevelItem: item

    QtObject {
        id: d

        property int sortTodosRole: page.C.ApplicationWindow.window.itemUtils.todosSortRoleFromString(
                                        AppSettings.todoListPageSettings.sortTodosBy)

        property var restoreLibraryUid
        property var restoreTodoListUid
        property var loadLibraryTransactionId
        property var loadTodoListTransactionId

        property bool editingNotes: false

        signal attach
        signal attachFiles(var fileUrls)
        signal openTagsEditor

        function openTodo(todo) {
            page.openPage(todoPage, {
                              "item": OTL.Application.cloneItem(todo)
                          })
        }
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

    ItemCreatedNotification {
        id: itemCreatedNotification
        onOpen: d.openTodo(item)
    }

    RenameItemDialog {
        id: renameItemDialog
    }

    OTL.ItemsSortFilterModel {
        id: todosModel
        sortRole: d.sortTodosRole
        groupDone: AppSettings.todoListPageSettings.groupDone
        sourceModel: OTL.ItemsModel {
            cache: OTL.Application.cache
            parentItem: page.item.uid
            searchString: filterBar.text
            onlyUndone: !AppSettings.todoListPageSettings.showUndone
        }
    }

    TextInputBar {
        id: filterBar
        placeholderText: qsTr("Search term 1, search term 2, ...")
        symbol: Icons.mdiClose
        showWhenNonEmpty: true
        closeOnButtonClick: true
    }

    C.Menu {
        id: sortTodosByMenu
        parent: todosWidget.headerIcon
        modal: true

        C.MenuItem {
            text: qsTr("Manually")
            checked: AppSettings.todoListPageSettings.sortTodosBy === "weight"
            checkable: true
            onTriggered: AppSettings.todoListPageSettings.sortTodosBy = "weight"
        }
        C.MenuItem {
            text: qsTr("Name")
            checked: AppSettings.todoListPageSettings.sortTodosBy === "title"
            checkable: true
            onTriggered: AppSettings.todoListPageSettings.sortTodosBy = "title"
        }
        C.MenuItem {
            text: qsTr("Due Date")
            checked: AppSettings.todoListPageSettings.sortTodosBy === "dueTo"
            checkable: true
            onTriggered: AppSettings.todoListPageSettings.sortTodosBy = "dueTo"
        }
        C.MenuItem {
            text: qsTr("Created At")
            checked: AppSettings.todoListPageSettings.sortTodosBy === "createdAt"
            checkable: true
            onTriggered: AppSettings.todoListPageSettings.sortTodosBy = "createdAt"
        }
        C.MenuItem {
            text: qsTr("Updated At")
            checked: AppSettings.todoListPageSettings.sortTodosBy === "updatedAt"
            checkable: true
            onTriggered: AppSettings.todoListPageSettings.sortTodosBy = "updatedAt"
        }
    }

    C.Menu {
        id: todosVisibilityMenu
        parent: todosWidget.headerIcon2
        modal: true

        C.MenuItem {
            text: qsTr("Show Completed")
            checked: AppSettings.todoListPageSettings.showUndone
            checkable: true
            onClicked: AppSettings.todoListPageSettings.showUndone
                       = !AppSettings.todoListPageSettings.showUndone
        }

        C.MenuItem {
            text: qsTr("Show At The End")
            checked: AppSettings.todoListPageSettings.groupDone
            checkable: true
            visible: AppSettings.todoListPageSettings.showUndone
            height: visible ? implicitHeight : 0
            onClicked: AppSettings.todoListPageSettings.groupDone
                       = !AppSettings.todoListPageSettings.groupDone
        }
    }

    ItemScrollView {
        id: scrollView

        anchors {
            left: parent.left
            right: parent.right
            top: filterBar.bottom
            bottom: parent.bottom
        }
        item: page.item
        C.ScrollBar.vertical.policy: d.editingNotes ? C.ScrollBar.AlwaysOn : C.ScrollBar.AsNeeded
        C.ScrollBar.vertical.interactive: true

        TodosWidget {
            id: todosWidget
            width: scrollView.availableWidth
            height: scrollView.availableHeight
            contentWidth: scrollView.availableWidth
            contentHeight: contentItem.childrenRect.height
            itemsModel: todosModel
            library: page.library
            title: qsTr("Todos")
            symbol: {
                switch (AppSettings.todoListPageSettings.sortTodosBy) {
                case "title":
                    return Icons.mdiSortByAlpha
                case "dueTo":
                    return Icons.mdiEvent
                case "createdAt":
                    // fall through
                case "updatedAt":
                    // fall through
                case "weight":
                    // fall through
                default:
                    return Icons.mdiFilterList
                }
            }
            symbol2: AppSettings.todoListPageSettings.showUndone ? Icons.mdiVisibility : Icons.mdiVisibilityOff
            headerItem2Visible: true
            allowCreatingNewItems: true
            newItemPlaceholderText: qsTr("Add new todo...")
            allowReordering: AppSettings.todoListPageSettings.sortTodosBy === "weight"
            allowSettingDueDate: true
            onHeaderButtonClicked: sortTodosByMenu.open()
            onHeaderButton2Clicked: todosVisibilityMenu.open()
            onTodoClicked: todo => d.openTodo(todo)
            onCreateNewItem: {
                var properties = {
                    "title": title
                }
                if (args.dueTo) {
                    properties.dueTo = args.dueTo
                }
                var todo = OTL.Application.addTodo(page.library, page.item,
                                                   properties)
                itemCreatedNotification.show(todo)
            }
            headerComponent: Column {
                id: column

                property alias itemNotesEditor: itemNotesEditor

                width: parent.width
                spacing: AppSettings.smallSpace

                ItemPageHeader {
                    counter: page.item.numDoneTodos
                    total: page.item.numTodos
                    item: page.item
                }

                TagsEditor {
                    id: tagsEditor
                    item: page.item
                    library: page.library
                    width: parent.width

                    Connections {
                        target: d
                        function onOpenTagsEditor() {
                            tagsEditor.addTag()
                        }
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
        refreshEnabled: page.library?.hasSynchronizer ?? false
        flickable: todosWidget
        onRefresh: OTL.Application.syncLibrary(page.library)
    }

    AutoScrollOverlay {
        anchors.fill: parent
        flickable: todosWidget
    }

    Component {
        id: todoPage
        TodoPage {
            library: page.library
            todoList: page.item
        }
    }

    Actions.CopyTopLevelItem {
        id: copyTopLevelItemAction
        item: page.item
        itemUtils: page.C.ApplicationWindow.window.itemUtils
    }

    Connections {
        target: OTL.Application

        function onLibraryLoaded(uid, data, transactionId) {
            if (uid === d.restoreLibraryUid
                    && transactionId == d.loadLibraryTransactionId) {
                page.library = OTL.Application.libraryFromData(data)
            }
        }

        function onItemLoaded(uid, data, parents, library, transactionId) {
            if (uid === d.restoreTodoListUid
                    && transactionId === d.loadTodoListTransactionId) {
                page.item = OTL.Application.itemFromData(data)
            }
        }
    }

    Tooltips.AllSubtasksDone {
        item: page.item
    }

    OTL.ShareUtils {
        id: shareUtils
    }
}
