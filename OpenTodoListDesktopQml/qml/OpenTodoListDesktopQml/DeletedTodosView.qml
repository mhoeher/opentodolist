import QtQuick 2.0

View {
    id: deletedTodosView

    property QtObject todoList: null

    hidden: !todoList

    toolButtons: [
        ToolButton {
            label: "Close"

            onClicked: deletedTodosView.todoList = null
        }

    ]

    ListView {
        width: deletedTodosView.clientWidth
        height: deletedTodosView.clientHeight
        model: deletedTodosView.todoList ? deletedTodosView.todoList.deletedTodos : null
        delegate: TodoListEntry {
            todo: object
        }
    }
}
