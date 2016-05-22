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
        tasks.focusNewItemInput();
    }

    function cancel() {
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
    
    RenameItemDialog {
        id: renameItemDialog
    }
    
    ScrollView {
        id: scrollView
        
        horizontalScrollBarPolicy: Qt.ScrollBarAlwaysOff
        anchors.fill: parent
        
        Column {
            width: scrollView.viewport.width
            spacing: Globals.defaultMargin
            
            MouseArea {
                onClicked: renameItemDialog.renameItem(todo)
                height: childrenRect.height
                width: parent.width
                cursorShape: Qt.PointingHandCursor
                
                Label {
                    text: todo.title
                    anchors {
                        left: parent.left
                        right: parent.right
                        margins: Globals.defaultMargin
                    }
                    font {
                        bold: true
                        pixelSize: Globals.fontPixelSize * 2
                    }
                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                }
            }
            
            TaskListView {
                id: tasks
                model: todo.tasks
                anchors {
                    left: parent.left
                    right: parent.right
                    margins: Globals.defaultMargin * 2
                }
                allowNewEntryCreation: true
                onAddEntry: {
                    var task = todo.addTask(title);
                }
            }
            
            StickyNote {
                id: note
                anchors {
                    left: parent.left
                    right: parent.right
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
            
            Item {
                height: Globals.defaultMargin
                anchors {
                    left: parent.left
                    right: parent.right
                    margins: Globals.defaultMargin * 2
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




