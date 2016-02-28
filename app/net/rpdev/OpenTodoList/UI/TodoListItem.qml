import QtQuick 2.0

import net.rpdev.OpenTodoList 1.0
import net.rpdev.OpenTodoList.UI 1.0

StickyNote {
    property TodoList libraryItem: TodoList {}
    
    title: libraryItem.title
    text: libraryItem.notes
    clip: true
    backgroundColor: Colors.itemColor(libraryItem.color)
    hoverEnabled: true
    checkBoxList: libraryItem.todos
    showCheckBoxList: true
}
