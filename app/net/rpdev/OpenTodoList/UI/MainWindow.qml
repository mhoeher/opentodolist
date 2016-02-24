import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Dialogs 1.2
import QtQuick.Window 2.0
import QtQuick.Layouts 1.1

import net.rpdev.OpenTodoList 1.0
import net.rpdev.OpenTodoList.UI 1.0

import "MainWindowLogic.js" as Logic

ApplicationWindow {
    id: window
    
    signal openLocalLibrary()
    
    title: qsTr("OpenTodoList")
    visible: true
    width: 640
    height: 480

    menuBar: MenuBar {
        Menu {
            title: qsTr("&Library")
            MenuItem {
                id: addLocalLibraryItem
                text: qsTr("&Add Local Library")
                shortcut: StandardKey.Open
                enabled: stack.depth <= 1
                onTriggered: openLocalLibraryDialog.open()
            }
            MenuItem {
                text: qsTr("&Exit")
                shortcut: StandardKey.Quit
                onTriggered: Qt.quit();
            }
        }
        
        Menu {
            id: itemsMenu
            
            title: qsTr("&Items")
            visible: leftSideBar.currentLibrary !== null
            
            MenuItem {
                id: newNoteItem
                text: qsTr("New &Note")
                enabled: stack.currentItem && typeof(stack.currentItem["newNote"]) === "function"
                shortcut: qsTr("Ctrl+Shift+N")
                onTriggered: stack.currentItem.newNote()
            }
            MenuItem {
                id: newImageItem
                text: qsTr("New &Image")
                shortcut: qsTr("Ctrl+Shift+I")
                enabled: stack.currentItem && typeof(stack.currentItem["newImage"]) === "function"
                onTriggered: stack.currentItem.newImage()
            }
            MenuItem {
                id: newTodoListItem
                text: qsTr("New &Todo List");
                shortcut: qsTr("Ctrl+Shift+T")
                enabled: stack.currentItem && typeof(stack.currentItem["newTodoList"]) === "function"
                onTriggered: stack.currentItem.newTodoList()
            }
            MenuItem {
                id: newTodoItem
                text: qsTr("New T&odo");
                shortcut: qsTr("Ctrl+Shift+O")
                enabled: stack.currentItem && typeof(stack.currentItem["newTodo"]) === "function"
                onTriggered: stack.currentItem.newTodo()
            }
        }
        
        Menu {
            id: editMenu
            
            title: qsTr("&Edit")
            visible: colorMenu.visible
            
            ColorMenu {
                id: colorMenu
                item: stack.currentItem && stack.currentItem["item"] ? stack.currentItem.item : null
            }
        }
        
        RichTextEditorFormatMenu {
            id: formatMenu
        }

        Menu {
            title: qsTr("Debug")
            visible: debugMode
            MenuItem {
                text: qsTr( "Reload UI")
                shortcut: StandardKey.Refresh
                enabled: qmlFileSystemWatcher.modified
                onTriggered: {
                    applicationWindow.close()
                    qmlFileSystemWatcher.reload()
                }
            }
        }
    }
    
    toolBar: ToolBar {
        height: Globals.minButtonHeight * 1.5
        
        RowLayout {
            width: parent.width
            height: parent.height
            
            ComboBox {
                id: libraryNavigator
                model: App.libraries
                editable: false
                textRole: "name"
                visible: !leftSideBar.visible
                height: parent.height
                onCurrentIndexChanged: leftSideBar.currentLibrary = App.libraries[currentIndex]
            }

            Symbol {
                symbol: Fonts.symbols.faAngleLeft
                visible: stack.currentItem && typeof(stack.currentItem["cancel"]) === "function"
                onClicked: Logic.cancelCurrent(stack)
            }
            Symbol {
                symbol: Fonts.symbols.faFolderOpenO
                visible: addLocalLibraryItem.enabled
                onClicked: addLocalLibraryItem.trigger()
            }
            Symbol {
                symbol: Fonts.symbols.faStickyNoteO
                visible: itemsMenu.visible && newNoteItem.enabled
                onClicked: newNoteItem.trigger()
            }
            Symbol {
                symbol: Fonts.symbols.faCheckSquareO
                visible: itemsMenu.visible && newTodoListItem.enabled
                onClicked: newTodoListItem.trigger()
            }
            Symbol {
                symbol: Fonts.symbols.faCheckSquareO
                visible: itemsMenu.visible && newTodoItem.enabled
                onClicked: newTodoItem.trigger()
            }
            Symbol {
                symbol: Fonts.symbols.faPictureO
                visible: itemsMenu.visible && newImageItem.enabled
                onClicked: newImageItem.trigger()
            }
            Symbol {
                symbol: Fonts.symbols.faPencilSquareO
                menu: ColorMenu {
                    visible: colorMenu.visible
                    item: colorMenu.item
                }
                visible: menu.visible
            }
            Item {
                Layout.fillWidth: true
            }
            Symbol {
                symbol: Fonts.symbols.faTrashO
                visible: stack.currentItem && typeof(stack.currentItem["deleteItem"]) === "function"
                onClicked: stack.currentItem.deleteItem()
            }
            Symbol {
                symbol: Fonts.symbols.faCheck
                visible: stack.currentItem && typeof(stack.currentItem["save"]) === "function"
                onClicked: Logic.saveCurrent(stack)
            }
        }
    }
    
    Component.onCompleted: {
        Globals.appWindow = window;
        width = App.loadValue("width", width);
        height = App.loadValue("height", height);
        if (App.loadValue("maximized", "false") === "true") {
            window.visibility = Window.Maximized
        }
        onVisibilityChanged.connect(function() {
            App.saveValue("maximized", visibility === Window.Maximized);
        });
        onWidthChanged.connect(function() {
            if (visibility === Window.Windowed) {
                App.saveValue("width", width);
            }
        });
        onHeightChanged.connect(function() {
            if (visibility === Window.Windowed) {
                App.saveValue("height", height);
            }
        });
    }
    
    onClosing: {
        if (Qt.platform.os == "android") {
            if (stack.depth > 0) {
                Logic.cancelCurrent(stack);
                close.accepted = false;
                console.error("Handled close request.");
                return;
            }
        }
        close.accepted = true;
    }
    
    Item {
        id: rootItem
        
        focus: true
        anchors.fill: parent
        
        
        onFocusChanged: {
            if (!activeFocusItem) {
                forceActiveFocus();
            }
        }
        Keys.onEscapePressed: Logic.goBack(stack)
        Keys.onBackPressed: Logic.goBack(stack)
        
        FileDialog {
            id: openLocalLibraryDialog
            title: qsTr("Open Local Library")
            selectFolder: true
            //folder: shortcuts.home //TODO: Why is this not working?
            selectExisting: false
            onAccepted: leftSideBar.currentLibrary = App.addLocalLibrary(fileUrl)
        }
        
        Image {
            anchors.fill: parent
            source: "cork_1/cork_1.png"
            fillMode: Image.Tile
        }
        
        LibrariesSideBar {
            id: leftSideBar
            anchors {
                top: parent.top
                bottom: parent.bottom
            }
            width: 200
            edge: Qt.LeftEdge
            x: applicationWindow.width > Globals.fontPixelSize * 40 ? 0 : -width
            visible: x > -width
            onCurrentLibraryChanged: Logic.viewLibrary(stack, currentLibrary, libraryPage)
        }
        
        StackView {
            id: stack
            anchors {
                left: leftSideBar.right
                top: parent.top
                right: parent.right
                bottom: parent.bottom
            }
            clip: true
        }
        
        MouseArea {
            anchors.fill: stack
            enabled: stack.busy
        }
        
        Component {
            id: libraryPage
            LibraryPage {
                onItemClicked: {
                    stack.push({item: Globals.file("/net/rpdev/OpenTodoList/UI/" +
                                      item.itemType + "Page.qml"),
                                   properties: { item: item, stack: stack } });
                }
                stackView: stack
            }
        }
    }
}
