import QtQuick 2.0
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.4
import QtQuick.Dialogs 1.2

import net.rpdev.OpenTodoList 1.0
import net.rpdev.OpenTodoList.UI 1.0

import "LibraryPageLogic.js" as Logic

Item {
    id: page
    
    property var library: null
    property StackView stackView: null
    
    signal itemClicked(TopLevelItem item)
    
    function newNote() {
        newNoteBar.edit.forceActiveFocus();
        newNoteBar.edit.text = "";
    }
    
    function newTodoList() {
        newTodoListBar.edit.forceActiveFocus();
        newTodoListBar.edit.text = "";
    }
    
    function newImage() {
        openImageDialog.open();
    }
    
    property var deleteItem: library === App.defaultLibrary ? null :
                                                              function deleteItem() {
                                                                  confirmDeleteLibrary.open();
                                                              }
    
    //function newImage() {}
    
    clip: true
    width: 100
    height: 100
    
    
    MessageDialog {
        id: confirmDeleteLibrary
        title: qsTr("Delete Library?")
        text: qsTr("Do you really want to remove the library <strong>%1</strong> from the " +
                   "application? Note that the files inside the library will not be removed, so " +
                   "you can restore the library later on.").arg(library.name)
        standardButtons: StandardButton.Ok | StandardButton.Cancel
        onAccepted: {
            library.deleteLibrary();
            stackView.pop();
        }
    }
    
    FileBrowser {
        id: openImageDialog
        stack: stackView
        title: qsTr("Add Image")
        selectExisting: true
        selectFolder: false
        selectMultiple: false
        folder: shortcuts.pictures
        nameFilters: ["*.jpg", "*.jpeg", "*.png", "*.bmp", "*.gif"]
        onAccepted: library.addImage(fileUrl)
    }
    
    TextInputBar {
        id: newNoteBar
        placeholderText: qsTr("Note Title")
        onAccepted: {
            var note = Logic.createNote(library, newNoteBar.edit);
            if (openItem) {
                itemClicked(note);
            }
        }
    }
    
    TextInputBar {
        id: newTodoListBar
        placeholderText: qsTr("Todo List Title")
        onAccepted: {
            var todoList = Logic.createTodoList(library, newTodoListBar.edit);
            if (openItem) {
                itemClicked(todoList);
            }
        }
    }
    
    ScrollView {
        id: scrollView
        anchors.fill: parent
        horizontalScrollBarPolicy: Qt.ScrollBarAlwaysOff
        
        
        Flow {
            width: scrollView.viewport.width
            flow: Flow.LeftToRight
            
            Repeater {
                id: repeater
                model: library ? library.items : null
                delegate: Loader {
                    width: Logic.sizeOfColumns(scrollView, Globals.minButtonHeight * 2)
                    height: width / 3 * 2
                    source: Globals.file("/net/rpdev/OpenTodoList/UI/" + 
                            library.items[index].itemType + "Item.qml")
                    
                    onLoaded: {
                        item.libraryItem = library.items[index];
                        item.onClicked.connect(function() {
                            itemClicked(item.libraryItem)
                        });
                    }
                }
            }
        }
    }
}
