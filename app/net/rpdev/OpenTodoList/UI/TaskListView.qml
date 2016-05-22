import QtQuick 2.0
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.1
import QtQuick.Dialogs 1.2

import net.rpdev.OpenTodoList 1.0
import net.rpdev.OpenTodoList.UI 1.0

Item {
    id: root
    
    property list<Task> model
    property bool allowNewEntryCreation: true
    
    signal addEntry(string title)
    
    function focusNewItemInput() {
        if (listView.headerItem) {
            listView.headerItem.focusInputItem();
        }
    }
    
    height: listView.contentHeight
    
    MessageDialog {
        id: confirmDeleteTaskDialog
        
        property Task task
        
        title: qsTr("Delete Task?")
        text: task ? qsTr("Do you want to delete the task <strong>%1</strong>? " +
                          "This cannot be undone.").arg(task.title) : ""
        standardButtons: StandardButton.Ok | StandardButton.Cancel
        onAccepted: task.deleteItem()
    }
    
    RenameItemDialog {
        id: renameItemDialog
    }

    Component {
        id: itemDelegate
        
        Item {
            width: parent.width
            height: row.height + Globals.defaultMargin * 2
            
            RowLayout {
                id: row
                
                width: parent.width
                anchors.verticalCenter: parent.verticalCenter
                
                Symbol {
                    symbol: done ? Fonts.symbols.faCheckSquareO : Fonts.symbols.faSquareO
                    onClicked: done = !done
                }
                
                MouseArea {
                    Layout.fillWidth: true
                    cursorShape: Qt.PointingHandCursor
                    onClicked: renameItemDialog.renameItem(todo.tasks[index])
                    height: childrenRect.height
                    
                    Label {
                        text: title
                        width: parent.width
                        wrapMode: Text.WrapAnywhere
                    }
                }
                
                Symbol {
                    symbol: Fonts.symbols.faTrashO
                    onClicked: {
                        confirmDeleteTodoDialog.todo = root.model[index];
                        confirmDeleteTodoDialog.open();
                    }
                }
            }
            
            Rectangle {
                width: parent.width
                height: 1
                color: background.border.color
            }
        }
    }
    
    Component {
        id: headerDelegate
        
        Item {
            
            function createItem() {
                newEntryEdit.accepted();
            }
            
            function focusInputItem() {
                newEntryEdit.forceActiveFocus();
            }
            
            width: parent.width
            height: row.height + Globals.defaultMargin * 2
            
            
            RowLayout {
                id: row
                
                width: parent.width
                anchors.verticalCenter: parent.verticalCenter
                
                Item {
                    width: Globals.minButtonHeight
                    height: Globals.minButtonHeight
                }
                
                TextField { 
                    id: newEntryEdit
                    Layout.fillWidth: true
                    placeholderText: qsTr("Add Task...")
                    style: TextFieldStyle {
                        background: Item {}
                        textColor: "black"
                    }
                    
                    onAccepted: {
                        if (displayText !== "") {
                            root.addEntry(displayText);
                            text = "";
                            forceActiveFocus();
                        }
                    }
                }
                
                Symbol {
                    symbol: Fonts.symbols.faPlus
                    onClicked: newEntryEdit.accepted()
                }
            }
            
            Rectangle {
                width: parent.width
                height: 1
                color: background.border.color
            }
        }
    }
    
    Rectangle {
        id: background
        border {
            width: 1
            color: "silver"
        }
        anchors.fill: parent
    }

    Image {
        id: backgroundImage
        anchors {
            fill: background
            margins: 1
        }
        source: "qrc:/net/rpdev/OpenTodoList/UI/soft_wallpaper/soft_wallpaper.png"        
        fillMode: Image.Tile
    }

    ListView {
        id: listView
        anchors {
            fill: parent
            leftMargin: Globals.defaultMargin
            rightMargin: Globals.defaultMargin
        }
        model: root.model
        delegate: itemDelegate
        header: allowNewEntryCreation ? headerDelegate : null
        clip: true
    }
}
