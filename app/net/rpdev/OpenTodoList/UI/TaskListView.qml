import QtQuick 2.0
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.1
import QtQuick.Dialogs 1.2

import net.rpdev.OpenTodoList 1.0
import net.rpdev.OpenTodoList.UI 1.0

Item {
    id: root
    
    property list<Task> model
    
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
                
                TextEdit {
                    text: title
                    Layout.fillWidth: true
                    wrapMode: Text.WrapAnywhere
                    Keys.onEnterPressed: focus = false
                    Keys.onReturnPressed: focus = false
                    Keys.onEscapePressed: focus = false
                    Keys.onBackPressed: focus = false
                    onFocusChanged: root.model[index].title = text
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
    }
}
