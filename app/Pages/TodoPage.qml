import QtQuick 2.5
import Qt.labs.settings 1.0

import OpenTodoList 1.0 as OTL

import "../Components"
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
    property Item parentDrawer: null

    signal closePage()
    signal openPage(var component, var properties)

    readonly property alias editingNotes: d.editingNotes

    function finishEditingNotes() {
        itemNotesEditor.finishEditing();
    }

    function deleteItem() {
        confirmDeleteDialog.deleteItem(item);
    }

    function deleteCompletedItems() {
        ItemUtils.deleteCompletedItems(item);
    }

    function renameItem() {
        renameItemDialog.renameItem(item);
    }

    function moveItem() {
        moveTodoAction.trigger();
    }

    function copyItem() {
        copyTodoAction.trigger();
    }

    function setProgress() {
        setManualProgressAction.trigger();
    }

    function setDueDate() {
        dueDateSelectionDialog.selectedDate = item.dueTo;
        dueDateSelectionDialog.open();
    }

    function find() {
        filterBar.edit.forceActiveFocus();
        d.reopenDrawer();
    }

    function attach() {
        d.attach();
    }

    property var goBack: editingNotes ? function() {
        itemNotesEditor.finishEditing();
    } : undefined

    property var undo: {
        if (d.savedTaskStates.length > 0) {
            return function() {
                let list = d.savedTaskStates.slice();
                OTL.Application.restoreItem(list.pop());
                d.savedTaskStates = list;
            };
        } else {
            return null;
        }
    }

    title: Markdown.markdownToPlainText(item.title)
    topLevelItem: todoList

    QtObject {
        id: d

        property bool editingNotes: false

        property var savedTaskStates: []

        signal attach()

        function reopenDrawer() {
            if (page.parentDrawer) {
                page.parentDrawer.open();
            }
        }
    }

    Actions.MoveTodo {
        id: moveTodoAction
        library: page.library
        item: page.item
    }

    Actions.SetManualProgressAction {
        id: setManualProgressAction
        item: page.item
    }

    DeleteItemDialog {
        id: confirmDeleteDialog
        onAccepted: {
            page.closePage();
        }
        onClosed: d.reopenDrawer()
    }

    DateSelectionDialog {
        id: dueDateSelectionDialog
        onAccepted: page.item.dueTo = selectedDate
        onClosed: d.reopenDrawer()
    }

    RenameItemDialog {
        id: renameItemDialog
        onClosed: d.reopenDrawer()
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
        C.ScrollBar.vertical.policy: itemNotesEditor.editing ? C.ScrollBar.AlwaysOn : C.ScrollBar.AsNeeded
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
                };
                var task = OTL.Application.addTask(
                            page.library,
                            page.item,
                            properties);
            }
            onItemSaved: {
                let list = d.savedTaskStates.slice();
                list.push(itemData);
                d.savedTaskStates = list;
            }

            headerComponent: Column {
                id: column
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
                        attach();
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
            onClicked: settings.showUndone = !settings.showUndone
        }

        C.MenuItem {
            text: qsTr("Show At The End")
            checked: settings.groupDone
            visible: settings.showUndone
            height: visible ? implicitHeight : 0
            onClicked: settings.groupDone = !settings.groupDone
        }
    }

    Actions.CopyTodo { id: copyTodoAction; item: page.item }
}

