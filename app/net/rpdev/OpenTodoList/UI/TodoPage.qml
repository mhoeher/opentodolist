import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Dialogs 1.2

import net.rpdev.OpenTodoList 1.0
import net.rpdev.OpenTodoList.UI 1.0

Item {
    id: page
    
    property Todo todo: Todo {}
    property StackView stack: null
    property bool __visible: Stack.status === Stack.Active
    
    function newTask() {
        newTaskBar.edit.forceActiveFocus();
        newTaskBar.edit.text = "";
    }

    function cancel() {
        todo.title = titleEdit.text;
    }
    
    function deleteItem() {
        confirmDeleteDialog.open();
    }
    
    MessageDialog {
        id: confirmDeleteDialog
        title: qsTr("Delete Todo?")
        text: qsTr("Are you sure you want to delete the todo <strong>%1</strong>? This action " +
                   "cannot be undone.").arg(todo.title)
        standardButtons: StandardButton.Ok | StandardButton.Cancel
        onAccepted: {
            todo.deleteItem();
            stack.pop();
        }
    }
    
    TextInputBar {
        id: newTaskBar
        placeholderText: qsTr("Task Title")
        onAccepted: {
            todo.addTask(newTaskBar.edit.text);
            newTaskBar.edit.focus = false;
        }
    }
    
    ScrollView {
        id: scrollView
        
        horizontalScrollBarPolicy: Qt.ScrollBarAlwaysOff
        anchors.fill: parent
        
        Item {
            width: scrollView.viewport.width
            height: childrenRect.height
            
            TextInput {
                id: titleEdit
                text: todo.title
                anchors {
                    left: parent.left
                    right: parent.right
                    top: parent.top
                    margins: Globals.defaultMargin
                }
                font {
                    bold: true
                    pixelSize: Globals.fontPixelSize * 2
                }
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                Keys.onEscapePressed: focus = false
                Keys.onBackPressed: focus = false
                Keys.onReturnPressed: focus = false
                Keys.onEnterPressed: focus = false
            }
            
            TaskListView {
                id: tasks
                model: todo.tasks
                anchors {
                    left: parent.left
                    right: parent.right
                    top: titleEdit.bottom
                    margins: Globals.defaultMargin * 2
                }
            }
            
            StickyNote {
                id: note
                anchors {
                    left: parent.left
                    right: parent.right
                    top: tasks.bottom
                    margins: Globals.defaultMargin
                }
                title: qsTr("Notes")
                text: todo.notes
                backgroundColor: Colors.itemYellow
                onClicked: {
                    var page = stack.push({ item: notesEditor, properties: { text: todo.notes }});
                    page.onTextChanged.connect(function() { todo.notes = page.text; });
                    todo.onReloaded.connect(function() { page.text = todo.notes; });
                }
            }
            
            Component {
                id: notesEditor
                
                RichTextEditor {
                    Component.onCompleted: forceActiveFocus()
                }
            }
        }
    }
}




