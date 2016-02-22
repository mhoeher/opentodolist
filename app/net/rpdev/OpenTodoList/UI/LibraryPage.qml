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
        newNoteTitle.forceActiveFocus();
        newNoteTitle.text = "";
    }
    
    function deleteItem() {
        confirmDeleteLibrary.open();
    }
    
    //function newTodoList() {}
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

    Rectangle {
        anchors.fill: newNoteBar
        color: Colors.headerBar
        z: 1
    }
    
    RowLayout {
        id: newNoteBar
        anchors {
            left: parent.left
            right: parent.right
        }
        y: newNoteTitle.focus ? 0 : -height
        z: 1
        height: childrenRect.height
        
        Behavior on y { SmoothedAnimation { duration: 500 } }
        
        TextField {
            id: newNoteTitle
            Layout.fillWidth: true
            Keys.onEscapePressed: focus = false
            Keys.onBackPressed: focus = false
            Keys.onEnterPressed: Logic.createNote(library, newNoteTitle)
            Keys.onReturnPressed: Logic.createNote(library, newNoteTitle)
            placeholderText: qsTr("Note Title")
        }
        
        Symbol {
            id: addNoteButton
            
            symbol: Fonts.symbols.faPlus
            enabled: newNoteTitle.text !== ""
            onClicked: Logic.createNote(library, newNoteTitle)
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
