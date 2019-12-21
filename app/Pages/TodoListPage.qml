import QtQuick 2.5
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12
import Qt.labs.settings 1.0

import QtQuick.Controls 2.12

import OpenTodoList 1.0 as OTL

import "../Components"
import "../Fonts"
import "../Windows"
import "../Widgets"
import "../Utils"

ItemPage {
    id: page

    property var library: null
    property OTL.TodoList item: OTL.TodoList {}

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
                                                  tagsEditor.addTag();
                                              }

    function attach() {
        if (todoDrawer.visible) {
            todoPage.attach();
        } else {
            attachments.attach();
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

    title: titleText.text
    topLevelItem: item

    MarkdownConverter {
        id: titleText
        markdown: item.title
        strip: true
    }

    QtObject {
        id: d

        function openTodo(todo) {
            todoPage.item = todo;
            todoDrawer.open();
        }

        property int sortTodosRole: {
            switch (settings.sortTodosBy) {
            case "title": return OTL.ItemsModel.TitleRole;
            case "dueTo": return OTL.ItemsModel.DueToRole;
            case "createdAt": return OTL.ItemsModel.CreatedAtRole;
            case "updatedAt": return OTL.ItemsModel.UpdatedAtRole;
            case "weight": // fall through
            default: return OTL.ItemsModel.WeightRole;
            }
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
        id: undoneTodos
        sortRole: d.sortTodosRole
        sourceModel: OTL.ItemsModel {
            cache: OTL.Application.cache
            parentItem: page.item.uid
            onlyUndone: true
            searchString: filterBar.text
        }
    }

    OTL.ItemsSortFilterModel {
        id: doneTodos
        sortRole: d.sortTodosRole
    }

    Binding {
        target: doneTodos
        property: "sourceModel"
        when: settings.showUndone
        value: OTL.ItemsModel {
            cache: OTL.Application.cache
            parentItem: page.item.uid
            onlyDone: true
            searchString: filterBar.text
        }
    }

    TextInputBar {
        id: filterBar
        placeholderText: qsTr("Search term 1, search term 2, ...")
        symbol: Icons.faTimes
        showWhenNonEmpty: true
        closeOnButtonClick: true
    }

    DateSelectionDialog {
        id: dialog
        onAccepted: page.item.dueTo = selectedDate
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
        padding: 10

        Column {
            width: parent.width
            spacing: 20

            ItemPageHeader {
                counter: undoneTodos.count
                item: page.item
            }

            TagsEditor {
                id: tagsEditor
                item: page.item
                library: page.library
                width: parent.width
            }

            ItemNotesEditor {
                id: itemNotesEditor
                item: page.item
                width: parent.width
            }

            TodosWidget {
                id: undoneTodosWidget
                width: parent.width
                model: undoneTodos
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
                allowCreatingNewItems: true
                newItemPlaceholderText: qsTr("Add new todo...")
                allowSorting: settings.sortTodosBy === "weight"
                allowSettingDueDate: true
                onHeaderButtonClicked: sortTodosByMenu.open()
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
            }

            Menu {
                id: sortTodosByMenu
                parent: undoneTodosWidget.headerItem
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

            TodosWidget {
                width: parent.width
                model: settings.showUndone ? doneTodos : null
                title: qsTr("Completed Todos")
                symbol: settings.showUndone ? Icons.faEye : Icons.faEyeSlash
                allowSorting: settings.sortTodosBy === "weight"
                onHeaderButtonClicked: settings.showUndone =
                                       !settings.showUndone
                onTodoClicked: d.openTodo(todo)
            }

            Attachments {
                id: attachments
                item: page.item
                width: parent.width
            }
        }
    }

    Drawer {
        id: todoDrawer

        edge: Qt.RightEdge
        width: page.width > 400 ? page.width / 3 * 2 : page.width
        height: page.height
        parent: MainWindow.contentItem
        clip: true
        dim: true
        onOpened: {
            todoPage.forceActiveFocus();
            interactive = true;
        }
        onClosed: {
            if (todoPage.editingNotes) {
                todoPage.finishEditingNotes();
            }
            interactive = false;
        }

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
    }
}

