import QtQuick 2.5
import QtQuick.Layouts 1.12
import Qt.labs.settings 1.0

import QtQuick.Controls 2.5

import OpenTodoList 1.0 as OTL

import "../Components"
import "../Fonts"
import "../Windows"
import "../Widgets"
import "../Utils"

Page {
    id: page

    property OTL.TodoList item: OTL.TodoList {}
    property var library: null

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

    function setDueDate(dueDate) {
        if (todoDrawer.visible) {
            todoPage.setDueDate(dueDate);
        } else {
            page.item.dueTo =  dueDate;
        }
    }

    function getDueDate() {
        if (todoDrawer.visible) {
            return todoPage.getDueDate();
        } else {
            return page.item.dueTo;
        }
    }

    property var goBack: todoDrawer.visible ? function() {
        todoDrawer.close();
    } : undefined

    title: titleText.text

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
    }

    Component.onDestruction: todoDrawer.close()

    Settings {
        id: settings
        category: "TodoListPage"

        property bool showUndone: false
    }

    DeleteItemDialog {
        id: confirmDeleteDialog
        onAccepted: {
            page.closePage();
        }
    }

    ItemCreatedNotification {
        id: itemCreatedNotification
        onOpen: d.openTodo(item)
    }

    RenameItemDialog {
        id: renameItemDialog
    }

    CenteredDialog {
        id: newTodoDialog
        title: qsTr("Add Todo")
        width: idealDialogWidth

        TextField {
            id: newTodoEdit

            width: newTodoDialog.availableWidth
            Keys.onEnterPressed: newTodoDialog.accept()
            Keys.onReturnPressed: newTodoDialog.accept()
        }

        standardButtons: Dialog.Ok | Dialog.Cancel

        onAccepted: {
            if (newTodoEdit.text !== "") {
                var properties = {
                    "title": newTodoEdit.displayText
                };
                var todo = OTL.Application.addTodo(
                            page.library, page.item, properties);
                itemCreatedNotification.show(todo);
            }
            newTodoEdit.text = "";
        }
        onRejected: {
            newTodoEdit.text = "";
        }
        onOpened: {
            newTodoEdit.forceActiveFocus()
        }
    }

    OTL.ItemsSortFilterModel {
        id: undoneTodos
        sourceModel: OTL.ItemsModel {
            cache: OTL.Application.cache
            parentItem: page.item.uid
            onlyUndone: true
            searchString: filterBar.text
        }
    }

    OTL.ItemsSortFilterModel {
        id: doneTodos
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

    Pane {
        anchors.fill: parent
        backgroundColor: Colors.color(Colors.itemColor(item), Colors.shade50)
    }

    ScrollView {
        id: scrollView

        anchors {
            left: parent.left
            right: parent.right
            top: filterBar.bottom
            bottom: parent.bottom
        }

        Pane {
            id: background

            backgroundColor: Colors.color(Colors.itemColor(item), Colors.shade50)
            width: scrollView.width

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
                    item: page.item
                    width: parent.width
                }

                TodosWidget {
                    width: parent.width
                    model: undoneTodos
                    title: qsTr("Todos")
                    symbol: Icons.faPlus
                    headerItemVisible: false
                    onHeaderButtonClicked: newTodoDialog.open()
                    onTodoClicked: d.openTodo(todo)
                }

                TodosWidget {
                    width: parent.width
                    model: settings.showUndone ? doneTodos : null
                    title: qsTr("Completed Todos")
                    symbol: settings.showUndone ? Icons.faEye : Icons.faEyeSlash
                    onHeaderButtonClicked: settings.showUndone =
                                           !settings.showUndone
                    onTodoClicked: d.openTodo(todo)
                }

                Attachments {
                    id: attachments
                    item: page.item
                    width: parent.width
                }

                Item {
                    height: newItemButton.height
                    width: 1
                }
            }
        }
    }

    NewItemButton {
        id: newItemButton

        onClicked: newTodoDialog.open()
    }

    MouseArea {
        id: pageBackground
        anchors.fill: parent
        enabled: todoDrawer.visible
        onClicked: todoDrawer.close()
    }

    Rectangle {
        // Use own dim effect. This is required, because under certain
        // circumstances, the dim is not removed when closing the
        // todo list page.
        anchors.fill: parent
        opacity: todoDrawer.position * 0.5
        color: "black"
    }

    Drawer {
        id: todoDrawer

        edge: Qt.RightEdge
        width: page.width > 400 ? page.width / 3 * 2 : page.width
        height: page.height
        parent: MainWindow.contentItem
        clip: true
        interactive: false
        modal: false
        dim: false // use own dim effect
        onOpened: todoPage.forceActiveFocus()

        TodoPage {
            id: todoPage

            width: todoDrawer.width
            height: todoDrawer.height
            anchors.fill: parent
            todoList: page.item
            library: page.library

            Action {
                enabled: todoDrawer.open
                shortcut: StandardKey.Back
                onTriggered: todoDrawer.close()
            }

            Action {
                enabled: todoDrawer.open
                shortcut: "Esc"
                onTriggered: todoDrawer.close()
            }

            Action {
                enabled: todoDrawer.open
                shortcut: "Back"
                onTriggered: todoDrawer.close()
            }
        }
    }
}

