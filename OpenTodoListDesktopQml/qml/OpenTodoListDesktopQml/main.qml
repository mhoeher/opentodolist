import QtQuick 2.0

Rectangle {
    id: root
    width: 800
    height: 600
    color: activePalette.window
    
    /*
     * Not working, see https://bugreports.qt-project.org/browse/QTBUG-26317
     *    SystemPalette { 
     *        id: activePalette
     }
     */
    // Workaround:
    Item {
        id: activePalette
        property color window: "#eeeeee"
        property color button: "#dddddd"
        property color buttonText: "#000000"
        
    }
    
    View {
        id: todoListView
        hidden: false
        
        toolButtons: [
        Button {
            label: "New List"
            
            onClicked: newTodoListView.hidden = false
        },
        Button {
            label: "Quit"
            
            onClicked: Qt.quit()
        }
        ]
        
        ListView {
            anchors.fill: parent
            model: library.todoLists
            delegate: Rectangle {
                width: parent.width
                height: todoListLabel.paintedHeight + 10
                border.width: 1
                border.color: 'black'
                
                Text {
                    id: todoListLabel
                    text: object.name
                }
                
                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        todoView.todoList = object;
                    }
                }
            }
        }
    }
    
    View {
        id: newTodoListView
        
        property alias name: newTodoListName.text
        property QtObject type: null
        
        toolButtons: [
        Button {
            label: "Create"
            
            onClicked: {
                if ( library.createTodoList( newTodoListView.name, newTodoListView.type ) ) {
                    newTodoListView.hidden = true;
                }
            }
        },
        Button {
            label: "Cancel"
            
            onClicked: newTodoListView.hidden = true
        }
        ]
        
        Grid {
            spacing: 5
            columns: 2
            anchors.fill: parent
            
            Text {
                text: "<b>List Name:</b>"
            }
            SimpleTextInput {
                id: newTodoListName
                width: parent.width * 0.8
            }
            Text {
                text: "<b>Type:</b>"
            }
            ListView {
                model: library.plugins.backends
                clip: true
                width: parent.width * 0.8
                height: 400
                delegate: Rectangle {
                    width: parent.width
                    height: childrenRect.height
                    radius: 5
                    color: object == newTodoListView.type ? Qt.darker( activePalette.button, 1.1 ) : activePalette.button
                    Text {
                        text: object.name
                    }
                    MouseArea {
                        anchors.fill: parent
                        onClicked: newTodoListView.type = object
                    }
                }
            }
        }
    }
    
    
    View {
        id: todoView
        hidden: todoList ? false : true
        
        property QtObject todoList: null
        
        toolButtons: [
        Button {
            label: "Close"
            onClicked: todoView.todoList = null
        }
        ]
        
        Column {
            anchors.fill: parent
            spacing: 5
            
            Row {
                spacing: 5
                id: addNewTodoRow
                
                SimpleTextInput {
                    id: newTodoTitle
                    width: todoView.width - addNewTodoButton.width - parent.spacing
                    text: ""
                }
                Button {
                    id: addNewTodoButton
                    label: "Add"
                    
                    onClicked: {
                        var todo = todoView.todoList.addTodo();
                        todo.title = newTodoTitle.text;
                        newTodoTitle.text = "";
                    }
                }
            }
            
            ListView {
                model: todoView.todoList ? todoView.todoList.entries : null
                width: parent.width
                height: 200
                clip: true
                delegate: TodoListEntry {
                    todo: object
                    onClicked: {
                        todoDetailsView.todo = object
                    }
                }
            }
        }
    }
    
    View {
        id: todoDetailsView
        hidden: todo ? false : true
        
        property QtObject todo: null
        
        toolButtons: [
            Button {
                label: "Back"
                
                onClicked: {
                    todoDetailsView.todo = todoDetailsView.todo.parentTodo
                }
            }
        ]
        
        Column {
            anchors.fill: parent
            spacing: 5
            
            Rectangle {
                height: childrenRect.height
                width: parent.width
                Text {
                    text: todoDetailsView.todo ? todoDetailsView.todo.title : ""
                }
            }
            
            Row {
                spacing: 5
                width: parent.width
                height: childrenRect.height
                
                ProgressBar {
                    value: todoDetailsView.todo ? todoDetailsView.todo.progress : 0
                }
                Button {
                    label: "-"
                    
                    onClicked: { todoDetailsView.todo.progress -= 10; }
                }
                Button {
                    label: "+"
                    
                    onClicked: { todoDetailsView.todo.progress += 10; }
                }
            }
            
            Row {
                width: parent.width
                
                SimpleTextInput {
                    id: newSubTodoTitle
                    text: ""
                    width: parent.width - addNewSubTodoButton.width
                }
                
                Button {
                    id: addNewSubTodoButton
                    label: "Add"
                    onClicked: {
                        var todo = todoDetailsView.todo.todoList.addTodo();
                        todo.title = newSubTodoTitle.text;
                        todo.parentTodo = todoDetailsView.todo;
                        newSubTodoTitle.text = "";
                    }
                }
            }
            
            ListView {
                model: todoDetailsView.todo ? todoDetailsView.todo.subTodos : null
                width: parent.width
                height: childrenRect.height
                clip: true
                delegate: TodoListEntry {
                    todo: object
                    onClicked: {
                        todoDetailsView.todo = object
                    }
                }
            }
            
            Rectangle {
                
                width: parent.width
                height: childrenRect.height
                
                MouseArea {
                    anchors.fill: parent    
                    onClicked: todoDescriptionEdit.focus = true;
                }
                
                TextEdit {
                    id: todoDescriptionEdit
                    
                    text: todoDetailsView.todo ? todoDetailsView.todo.description : ""
                    textFormat: TextEdit.RichText
                    wrapMode: Text.Wrap
                    width: parent.width
                }
            }
            
            Button {
                label: "Save"
                anchors.right: parent.right
                height: 30
                
                onClicked: { todoDetailsView.todo.description = todoDescriptionEdit.text; }
            }
            
        }
    }
    
    
}
             