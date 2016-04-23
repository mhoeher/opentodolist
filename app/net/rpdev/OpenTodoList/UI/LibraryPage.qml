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
    property string tag: ""
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
        fileNameExtensions: ["jpg", "jpeg", "png", "bmp", "gif"]
        onAccepted: {
            var image = library.addImage(fileUrl);
            if (leftSideBar.currentTag !== "") {
                image.addTag(leftSideBar.currentTag);
            }
        }
    }
    
    TopLevelItemsModel {
        id: itemsModel
        
        library: page.library
    }
    
    FilterModel {
        id: filteredItemsModel
        sourceModel: itemsModel
        filterFunction: (page.tag === "") ? null : function(i) {
            var result = false;
            var item = sourceModel.data(sourceModel.index(i, 0), TopLevelItemsModel.ObjectRole);
            for (var j = 0; j < item.tags.length; ++j) {
                if (item.tags[j] === page.tag) {
                    result = true;
                    break;
                }
            }
            return result;
        }
    }
    
    TextInputBar {
        id: newNoteBar
        placeholderText: qsTr("Note Title")
        onAccepted: {
            var note = Logic.createNote(library, newNoteBar.edit);
            if (openItem) {
                itemClicked(note);
            }
            if (leftSideBar.currentTag !== "") {
                note.addTag(leftSideBar.currentTag)
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
            if (leftSideBar.currentTag !== "") {
                todoList.addTag(leftSideBar.currentTag)
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
                model: filteredItemsModel
                delegate: Loader {
                    width: Logic.sizeOfColumns(scrollView, Globals.minButtonHeight * 2)
                    height: width / 3 * 2
                    source: Globals.file("/net/rpdev/OpenTodoList/UI/" + 
                            object.itemType + "Item.qml")
                    
                    onLoaded: {
                        item.libraryItem = object;
                        item.onClicked.connect(function() {
                            itemClicked(item.libraryItem);
                        });
                    }
                }
            }
        }
    }
}
