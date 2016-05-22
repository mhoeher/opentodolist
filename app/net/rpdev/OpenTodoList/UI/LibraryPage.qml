import QtQuick 2.0
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtQuick.Dialogs 1.2
import QtQuick.Window 2.2

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
    
    function find() {
        filterBar.edit.forceActiveFocus()
    }
    
    property var deleteItem: library === App.defaultLibrary ? null :
                                                              function deleteItem() {
                                                                  confirmDeleteLibrary.open();
                                                              }
    
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
    
    MessageDialog {
        id: confirmDeleteItem
        title: qsTr("Delete Item?")
        text: qsTr("Do you really want to delete the item <strong>%1</strong> from the library? " +
                   "This action cannot be undone!").arg(
                  itemContextMenu.item ? itemContextMenu.item.title : "")
        standardButtons: StandardButton.Ok | StandardButton.Cancel
        onAccepted: itemContextMenu.item.deleteItem()
    }
    
    RenameItemDialog {
        id: renameItemDialog
    }
    
    CreateNewTagDialog {
        id: createNewTagDialog
    }
    
    Menu {
        id: itemContextMenu
        
        property TopLevelItem item: null
        
        onItemChanged: tagsMenu.rebuild()
        
        Menu {
            title: qsTr("Color")
            
            MenuItem {
                text: qsTr("Red")
                onTriggered: itemContextMenu.item.color = TopLevelItem.Red
            }
            MenuItem {
                text: qsTr("Green")
                onTriggered: itemContextMenu.item.color = TopLevelItem.Green
            }
            MenuItem {
                text: qsTr("Blue")
                onTriggered: itemContextMenu.item.color = TopLevelItem.Blue
            }
            MenuItem {
                text: qsTr("Yellow")
                onTriggered: itemContextMenu.item.color = TopLevelItem.Yellow
            }
            MenuItem {
                text: qsTr("Orange")
                onTriggered: itemContextMenu.item.color = TopLevelItem.Orange
            }
            MenuItem {
                text: qsTr("Lilac")
                onTriggered: itemContextMenu.item.color = TopLevelItem.Lilac
            }
            MenuItem {
                text: qsTr("White")
                onTriggered: itemContextMenu.item.color = TopLevelItem.White
            }
        }
        
        MenuItem {
            text: qsTr("Rename")
            onTriggered: renameItemDialog.renameItem(itemContextMenu.item)
        }
        
        Menu {
            id: tagsMenu
            
            title: qsTr("Tags")
            
            function rebuild() {
                while (items.length > 0) {
                    removeItem(items[0]);
                }
                var topLevelItem = itemContextMenu.item;
                var item = addItem(qsTr("Create new tag..."));
                item.onTriggered.connect(function() {
                    createNewTagDialog.addTag(topLevelItem);
                });
                
                var tags = page.library.tags;
                for (var i = 0; i < tags.length; ++i) {
                    var tag = tags[i];
                    __addTagMenuItem(tag);
                }
            }
            
            function __addTagMenuItem(tag) {
                var topLevelItem = itemContextMenu.item;
                var item = addItem(tag);
                item.checkable = true;
                item.checked = Qt.binding(function() {
                    return topLevelItem.tags.indexOf(tag) >= 0; 
                });
                item.onTriggered.connect(function() {
                    if (topLevelItem.hasTag(tag)) {
                        topLevelItem.removeTag(tag);
                    } else {
                        topLevelItem.addTag(tag);
                    }
                });
            }
            
            Connections {
                target: page.library
                onTagsChanged: tagsMenu.rebuild(); 
            }
        }

        MenuItem {
            text: qsTr("Delete")
            onTriggered: confirmDeleteItem.open()
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
        
        function __tagMatches(item) {
            return (page.tag === "") || item.hasTag(page.tag)
        }
        
        sourceModel: itemsModel
        filterFunction: function(i) {
            var item = sourceModel.data(sourceModel.index(i, 0), TopLevelItemsModel.ObjectRole);
            return __tagMatches(item);
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
    
    TextInputBar {
        id: filterBar
        placeholderText: qsTr("Search term 1, search term 2, ...")
        symbol: Fonts.symbols.faTimes
        color: Colors.secondary2
        itemCreator: false
        showWhenNonEmpty: true
        closeOnButtonClick: true
        
        onTextChanged: {
            function matches(item) {
                var filter = filterBar.text;
                return Logic.itemMatchesFilterWithDefault(item, filter, true);
            }
            filteredItemsModel.filterFunction = function(i) {
                var item = filteredItemsModel.sourceModel.data(
                            filteredItemsModel.sourceModel.index(i, 0),
                            TopLevelItemsModel.ObjectRole);
                return filteredItemsModel.__tagMatches(item) &&
                        matches(item);
            }
        }
    }
    
    ScrollView {
        id: scrollView
        anchors {
            left: parent.left
            right: parent.right
            bottom: parent.bottom
            top: filterBar.bottom
            topMargin: filterBar.shown ? filterBar.contentHeight - filterBar.height : 0
        }
        style: ScrollViewStyle {
            transientScrollBars: true
        }

        Flow {
            width: scrollView.viewport.width
            flow: Flow.LeftToRight
            
            Repeater {
                id: repeater
                model: filteredItemsModel
                delegate: Loader {
                    width: Logic.sizeOfColumns(scrollView)
                    height: width / 3 * 2
                    source: Globals.file("/net/rpdev/OpenTodoList/UI/" + 
                            object.itemType + "Item.qml")
                    
                    onLoaded: {
                        item.libraryItem = object;
                        item.onClicked.connect(function() {
                            
                        });
                        item.onReleased.connect(function(mouse) {
                            switch (mouse.button) {
                            case Qt.LeftButton:
                                itemClicked(item.libraryItem);
                                break;
                            case Qt.RightButton:
                                itemContextMenu.item = item.libraryItem;
                                itemContextMenu.popup();
                                break;
                            default:
                                break;
                            }
                        });
                    }
                }
            }
        }
    }
}
