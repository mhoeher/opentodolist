import QtQuick 2.5
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.0
import Qt.labs.settings 1.0

import OpenTodoList 1.0 as OTL

import "../Components"
import "../Fonts"
import "../Windows"
import "../Widgets"
import "../Utils"
import "../Menues"

ItemPage {
    id: page

    property var library: null
    property OTL.TodoList item: OTL.TodoList {}
    property alias pageActions: libraryActions.actions

    signal closePage()
    signal openPage(var component, var properties)

    function deleteItem() {
        if (todoDrawer.visible) {
            todoPage.deleteItem();
        } else {
            confirmDeleteDialog.deleteItem(item);
        }
    }

    function renameItem() {
        if (todoDrawer.visible) {
            todoPage.renameItem();
        } else {
            renameItemDialog.renameItem(item);
        }
    }

    function find() {
        if (todoDrawer.visible) {
            todoPage.find();
        } else {
            filterBar.edit.forceActiveFocus();
        }
    }

    property var addTag: todoDrawer.visible ? undefined :
                                              function() {
                                                  d.openTagsEditor();
                                              }

    function attach() {
        if (todoDrawer.visible) {
            todoPage.attach();
        } else {
            d.attach();
        }
    }

    function setDueDate() {
        if (todoDrawer.visible) {
            todoPage.setDueDate();
        } else {
            dueDateSelectionDialog.selectedDate = item.dueTo;
            dueDateSelectionDialog.open();
        }
    }

    property var goBack: todoDrawer.visible ?
                             function() {
                                 todoDrawer.close();
                             } : null

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

        signal attach()
        signal openTagsEditor()

        function openTodo(todo) {
            todoPage.item = todo;
            todoDrawer.open();
        }

    }

    Component.onDestruction: todoDrawer.close()

    Settings {
        id: settings
        category: "TodoListPage"

        property bool showUndone: false
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
        symbol: Icons.faTimes
        showWhenNonEmpty: true
        closeOnButtonClick: true
    }

    Menu {
        id: sortTodosByMenu
        parent: todosWidget.headerIcon
        modal: true

        MenuItem {
            text: qsTr("Manually")
            checked: settings.sortTodosBy === "weight"
            checkable: true
            onTriggered: settings.sortTodosBy = "weight"
        }
        MenuItem {
            text: qsTr("Name")
            checked: settings.sortTodosBy === "title"
            checkable: true
            onTriggered: settings.sortTodosBy = "title"
        }
        MenuItem {
            text: qsTr("Due Date")
            checked: settings.sortTodosBy === "dueTo"
            checkable: true
            onTriggered: settings.sortTodosBy = "dueTo"
        }
        MenuItem {
            text: qsTr("Created At")
            checked: settings.sortTodosBy === "createdAt"
            checkable: true
            onTriggered: settings.sortTodosBy = "createdAt";
        }
        MenuItem {
            text: qsTr("Updated At")
            checked: settings.sortTodosBy === "updatedAt"
            checkable: true
            onTriggered: settings.sortTodosBy = "updatedAt";
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

        TodosWidget {
            id: todosWidget
            width: scrollView.contentItem.width
            model: todosModel
            title: qsTr("Todos")
            symbol: {
                switch (settings.sortTodosBy) {
                case "title":
                    return Icons.faSortAlphaDown;
                case "dueTo":
                    return Icons.faSortNumericDown;
                case "createdAt":
                    return Icons.faSortNumericDown;
                case "updatedAt":
                    return Icons.faSortNumericDown;
                case "weight":
                    // fall through
                default:
                    return Icons.faSort;
                }
            }
            symbol2: settings.showUndone ? Icons.faEye : Icons.faEyeSlash
            headerItem2Visible: true
            allowCreatingNewItems: true
            newItemPlaceholderText: qsTr("Add new todo...")
            allowSorting: settings.sortTodosBy === "weight"
            allowSettingDueDate: true
            onHeaderButtonClicked: sortTodosByMenu.open()
            onHeaderButton2Clicked: settings.showUndone = !settings.showUndone
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
            headerComponent: Column {
                id: column

                width: parent.width
                spacing: 20

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
                        onOpenTagsEditor: tagsEditor.addTag()
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
                    onAttach: attach()
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

    Pane {
        anchors.fill: parent
        visible: todoDrawer.visible
        opacity: 0.6 * todoDrawer.position
        Material.background: Colors.materialOverlayDimColor

        MouseArea {
            anchors.fill: parent
            enabled: todoDrawer.visible
            onClicked: todoDrawer.close()
        }
    }

    Pane {
        id: todoDrawer

        function open() {
            state = "open";
            forceActiveFocus();
        }

        function close() {
            state = "closed";
            if (todoPage.editingNotes) {
                todoPage.finishEditingNotes();
            }
        }

        readonly property double position: {
            var openX = page.width - todoDrawer.width;
            var closedX = page.width;
            return 1 - (openX - x) / (openX - closedX);
        }

        state: "closed"

        Material.elevation: visible ? 10 : 0

        width: page.width > 400 ? page.width / 3 * 2 : page.width
        height: page.height
        clip: true
        x: parent.width
        visible: false
        padding: 0

        TodoPage {
            id: todoPage

            width: todoDrawer.width
            height: todoDrawer.height
            anchors.fill: parent
            todoList: page.item
            library: page.library
            parentDrawer: todoDrawer

            function backAction() {
                if (editingNotes) {
                    finishEditingNotes();
                } else {
                    todoDrawer.close();
                }
            }
        }

        states: State {
            name: "open"

            PropertyChanges {
                target: todoDrawer
                visible: true
                x: page.width - todoDrawer.width
            }
        }

        transitions: Transition {
            from: "closed"
            to: "open"
            reversible: true

            SequentialAnimation {
                PropertyAnimation {
                    properties: "visible"
                }
                NumberAnimation {
                    properties: "x"
                    duration: 100
                }
            }
        }
    }
}

