import QtQuick 2.5
import QtQuick.Controls 2.2

import OpenTodoList 1.0
import OpenTodoList.UI 1.0

Item {
    id: page
    
    property Note item: Note {}
    property var library: null

    signal closePage()
    signal openPage(var component, var properties)

    
    function cancel() {
    }
    
    function deleteItem() {
        confirmDeleteDialog.open();
    }
    
    CenteredDialog {
        id: confirmDeleteDialog
        title: qsTr("Delete Note?")

        Text {
            text: qsTr("Are you sure you want to delete the note <strong>%1</strong>? This action " +
                       "cannot be undone.").arg(item.displayTitle)
            anchors {
                left: parent.left
                right: parent.right
                top: parent.top
                margins: Globals.defaultMargin
            }
            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
        }

        standardButtons: Dialog.Ok | Dialog.Cancel
        onAccepted: {
            item.deleteItem();
            page.closePage();
        }
    }
    
    Rectangle {
        color: Qt.lighter(Colors.itemColor(item.color), 1.1)
        opacity: 0.3
        anchors.fill: parent
    }
    
    ScrollView {
        id: scrollView
        
        anchors.fill: parent
        
        Column {
            width: scrollView.width
            spacing: Globals.defaultMargin
            
            ItemTitle {
                item: page.item
                width: parent.width
            }
            
            StickyNote {
                id: note
                anchors {
                    left: parent.left
                    right: parent.right
                    margins: Globals.defaultMargin
                }
                title: qsTr("Notes")
                text: Globals.markdownToHtml(item.notes)
                backgroundColor: item.color === TopLevelItem.White ? Colors.noteBackground : Colors.itemWhite
                onClicked: {
                    page.openPage(notesEditor, {"item": item});
                }
            }
            
            Component {
                id: notesEditor
                
                RichTextEditor {
                    id: editor
                }
            }

            Attachments {
                item: page.item
                anchors {
                    left: parent.left
                    right: parent.right
                    margins: Globals.defaultMargin * 2
                }
            }
            
            TagsEditor {
                item: page.item
                library: page.library
                anchors {
                    left: parent.left
                    right: parent.right
                    margins: Globals.defaultMargin * 2
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
        }
    }
}




