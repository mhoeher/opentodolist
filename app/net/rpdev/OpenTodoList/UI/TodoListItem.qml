import QtQuick 2.0

import net.rpdev.OpenTodoList 1.0
import net.rpdev.OpenTodoList.UI 1.0

StickyNote {
    id: todoListItem
    
    property TodoList libraryItem: null
    
    title: libraryItem.title
    text: libraryItem.notes
    clip: true
    backgroundColor: Colors.itemColor(libraryItem.color)
    hoverEnabled: true
    showCheckBoxList: true
    checkBoxList: FilterModel {
        sourceModel: TodosModel {
            todoList: libraryItem
        }
        filterFunction: function(index) {
            var i = sourceModel.index(index, 0);
            var todo = sourceModel.data(i, TodosModel.ObjectRole);
            return todo && !todo.done;
        }
    }
    
}
