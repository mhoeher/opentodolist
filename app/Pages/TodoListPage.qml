import QtQuick 2.5
import Qt.labs.settings 1.0

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
        confirmDeleteDialog.deleteItem(item);
    }

    function renameItem() {
        renameItemDialog.renameItem(item);
    }

    title: OTL.Application.htmlToPlainText(Markdown.format(item.title))

    QtObject {
        id: d

        function openTodo(todo) {
            page.openPage(
                        Qt.resolvedUrl("TodoPage.qml"),
                        {
                            item: todo,
                            todoList: page.item,
                            library: page.library
                        });
        }
    }

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

    ItemCreatedNotification { id: itemCreatedNotification }

    RenameItemDialog {
        id: renameItemDialog
    }

    CenteredDialog {
        id: newTodoDialog
        title: qsTr("Add Todo")

        TextField {
            id: newTodoEdit

            Keys.onEnterPressed: newTodoDialog.accept()
            Keys.onReturnPressed: newTodoDialog.accept()
        }

        standardButtons: Dialog.Ok | Dialog.Cancel

        onAccepted: {
            if (newTodoEdit.text !== "") {
                var todo = page.item.addTodo();
                todo.title = newTodoEdit.text;
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

    OTL.ItemsModel {
        id: todos
        container: page.library.todos
    }

    OTL.ItemsSortFilterModel {
        id: undoneTodos
        sourceModel: todos
        todoList: page.item.uid
        onlyUndone: true
    }

    OTL.ItemsSortFilterModel {
        id: doneTodos
        sourceModel: todos
        todoList: page.item.uid
        onlyDone: true
    }

    Pane {
        id: background

        backgroundColor: Colors.color(Colors.itemColor(item), Colors.shade50)
        anchors.fill: parent

        ScrollView {
            id: scrollView

            anchors.fill: parent

            Column {
                width: scrollView.width

                TodosWidget {
                    width: parent.width
                    model: undoneTodos
                    title: qsTr("Todos")
                    decorativeSymbol: Icons.faCircle
                    decorativeSymbolFont: Fonts.icons
                    symbol: Icons.faPlus
                    onHeaderButtonClicked: newTodoDialog.open()
                    onTodoClicked: d.openTodo(todo)
                }

                TodosWidget {
                    width: parent.width
                    model: settings.showUndone ? doneTodos : null
                    title: qsTr("Completed Todos")
                    decorativeSymbol: Icons.faCheckCircle
                    decorativeSymbolFont: Fonts.icons
                    symbol: settings.showUndone ? Icons.faEye : Icons.faEyeSlash
                    onHeaderButtonClicked: settings.showUndone =
                                           !settings.showUndone
                    onTodoClicked: d.openTodo(todo)
                }

                ItemNotesEditor {
                    item: page.item
                    width: parent.width
                }

                ItemDueDateEditor {
                    item: page.item
                    width: parent.width
                }

                Attachments {
                    item: page.item
                    width: parent.width
                }

                TagsEditor {
                    item: page.item
                    library: page.library
                    width: parent.width
                }
            }
        }
    }
}

