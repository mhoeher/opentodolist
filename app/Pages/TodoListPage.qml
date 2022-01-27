import QtQuick 2.5
import QtQuick.Layouts 1.12
import QtQuick.Controls.Material 2.0
import Qt.labs.settings 1.0

import OpenTodoList 1.0 as OTL

import "../Components"
import "../Controls" as C
import "../Fonts"
import "../Windows"
import "../Widgets"
import "../Utils"
import "../Menues"
import "../Actions" as Actions

ItemPage {
    id: page

    property var library: null
    property OTL.TodoList item: OTL.TodoList {}
    property alias pageActions: libraryActions.actions

    signal closePage()
    signal openPage(var component, var properties)

    function deleteItem() {
        confirmDeleteDialog.deleteItem(item);
    }

    function deleteCompletedItems() {
        ItemUtils.deleteCompletedItems(item);
    }

    function renameItem() {
        renameItemDialog.renameItem(item);
    }

    function copyItem() {
        copyTopLevelItemAction.trigger();
    }

    function find() {
        filterBar.edit.forceActiveFocus();
    }

    property var goBack: (todosWidget.header.itemNotesEditor || {}).editingNotes ? function() {
        todosWidget.header.itemNotesEditor.finishEditing();
    } : undefined

    property var undo: {
        if (d.savedTodoStates.length > 0) {
            return function() {
                if (d.savedTodoStates.length > 0) {
                    let list = d.savedTodoStates.slice();
                    let todoData = list.pop();
                    OTL.Application.restoreItem(todoData);
                    d.savedTodoStates = list;
                }
            };
        } else {
            return null;
        }
    }

    function addTag() {
        d.openTagsEditor();
    }

    function attach() {
        d.attach();
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

    QtObject {
        id: d

        property int sortTodosRole: {
            switch (settings.sortTodosBy) {
            case "title": return OTL.ItemsModel.TitleRole;
            case "dueTo": return OTL.ItemsModel.EffectiveDueToRole;
            case "createdAt": return OTL.ItemsModel.CreatedAtRole;
            case "updatedAt": return OTL.ItemsModel.UpdatedAtRole;
            case "weight": // fall through
            default: return OTL.ItemsModel.WeightRole;
            }
        }

        property var savedTodoStates: []

        signal attach()
        signal openTagsEditor()

        function openTodo(todo) {
            page.openPage(todoPage, {
                              item: OTL.Application.cloneItem(todo)
                          });
        }

    }

    Settings {
        id: settings
        category: "TodoListPage"

        property bool showUndone: false
        property bool groupDone: false
        property string sortTodosBy: "weight"
    }

    DeleteItemDialog {
        id: confirmDeleteDialog
        onAccepted: {
            page.closePage();
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

    C.Menu {
        id: sortTodosByMenu
        parent: todosWidget.headerIcon
        modal: true

        C.MenuItem {
            text: qsTr("Manually")
            checked: settings.sortTodosBy === "weight"
            checkable: true
            onTriggered: settings.sortTodosBy = "weight"
        }
        C.MenuItem {
            text: qsTr("Name")
            checked: settings.sortTodosBy === "title"
            checkable: true
            onTriggered: settings.sortTodosBy = "title"
        }
        C.MenuItem {
            text: qsTr("Due Date")
            checked: settings.sortTodosBy === "dueTo"
            checkable: true
            onTriggered: settings.sortTodosBy = "dueTo"
        }
        C.MenuItem {
            text: qsTr("Created At")
            checked: settings.sortTodosBy === "createdAt"
            checkable: true
            onTriggered: settings.sortTodosBy = "createdAt";
        }
        C.MenuItem {
            text: qsTr("Updated At")
            checked: settings.sortTodosBy === "updatedAt"
            checkable: true
            onTriggered: settings.sortTodosBy = "updatedAt";
        }
    }

    C.Menu {
        id: todosVisibilityMenu
        parent: todosWidget.headerIcon2
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

    ItemScrollView {
        id: scrollView

        anchors {
            left: parent.left
            right: parent.right
            top: filterBar.bottom
            bottom: parent.bottom
        }
        item: page.item
        C.ScrollBar.vertical.policy: (todosWidget.header.itemNotesEditor || {}).editing ? C.ScrollBar.AlwaysOn : C.ScrollBar.AsNeeded
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
                switch (settings.sortTodosBy) {
                case "title":
                    return Icons.mdiSortByAlpha;
                case "dueTo":
                    return Icons.mdiEvent;
                case "createdAt":
                    // fall through
                case "updatedAt":
                    // fall through
                case "weight":
                    // fall through
                default:
                    return Icons.mdiFilterList;
                }
            }
            symbol2: settings.showUndone ? Icons.mdiVisibility : Icons.mdiVisibilityOff
            headerItem2Visible: true
            allowCreatingNewItems: true
            newItemPlaceholderText: qsTr("Add new todo...")
            allowSorting: settings.sortTodosBy === "weight"
            allowSettingDueDate: true
            onHeaderButtonClicked: sortTodosByMenu.open()
            onHeaderButton2Clicked: todosVisibilityMenu.open()
            onTodoClicked: d.openTodo(todo)
            onCreateNewItem: {
                var properties = {
                    "title": title,
                };
                if (args.dueTo) {
                    properties.dueTo = args.dueTo
                }
                var todo = OTL.Application.addTodo(
                            page.library, page.item, properties);
                itemCreatedNotification.show(todo);
            }
            onItemSaved: {
                let list = d.savedTodoStates.slice();
                list.push(itemData);
                d.savedTodoStates = list;
            }
            headerComponent: Column {
                id: column

                property alias itemNotesEditor: itemNotesEditor

                width: parent.width
                spacing: AppSettings.smallSpace

                ItemPageHeader {
                    counter: todosModel.count
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
                            tagsEditor.addTag();
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

    Component {
        id: todoPage
        TodoPage { library: page.library; todoList: page.item }
    }

    Actions.CopyTopLevelItem { id: copyTopLevelItemAction; item: page.item }
}

