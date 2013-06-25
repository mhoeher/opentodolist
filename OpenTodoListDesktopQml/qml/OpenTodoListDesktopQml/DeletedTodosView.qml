import QtQuick 2.0
import net.rpdev.OpenTodoList 1.0

View {
    id: deletedTodosView

    property QtObject todoList: null
    property TodoSortFilterModel model: TodoSortFilterModel {
        sourceModel: todoList ? todoList.todos : null
        filterMode: TodoSortFilterModel.HideNonDeleted
    }

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
        model: deletedTodosView.model
        delegate: TodoListEntry {
            todo: object
        }
    }
}
