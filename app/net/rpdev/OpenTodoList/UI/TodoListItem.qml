import QtQuick 2.0

import net.rpdev.OpenTodoList 1.0
import net.rpdev.OpenTodoList.UI 1.0

StickyNote {
    id: todoListItem
    
    property TodoList libraryItem: TodoList {}
    
    function __updateTodos() {
        checkBoxList = libraryItem.todos
    }
    
    title: libraryItem.title
    text: libraryItem.notes
    clip: true
    backgroundColor: Colors.itemColor(libraryItem.color)
    hoverEnabled: true
    showCheckBoxList: true
    
    onLibraryItemChanged: __updateTodos()
    
    Connections {
        target: libraryItem
        onTodosChanged: __updateTodos()
    }
}
