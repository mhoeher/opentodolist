import QtQuick 2.4
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
            }
            MenuItem {
                id: newTodoListItem
                text: qsTr("New &Todo List");
                shortcut: qsTr("Ctrl+Shift+T")
                enabled: stack.currentItem && typeof(stack.currentItem["newTodoList"]) === "function"
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
        RowLayout {
            width: parent.width
            
            ComboBox {
                id: libraryNavigator
                model: App.libraries
                editable: false
                textRole: "name"
                visible: !leftSideBar.visible
                onCurrentIndexChanged: leftSideBar.currentLibrary = App.libraries[currentIndex]
            }

            ToolButton {
                iconSource: "qrc:/FontAwesome/black/angle-left.svg"
                visible: stack.currentItem && typeof(stack.currentItem["cancel"]) === "function"
                onClicked: Logic.cancelCurrent(stack)
            }
            ToolButton {
                iconSource: "qrc:/FontAwesome/black/folder-open-o.svg"
                visible: addLocalLibraryItem.enabled
                onClicked: addLocalLibraryItem.trigger()
            }
            ToolButton {
                iconSource: "qrc:/FontAwesome/black/sticky-note-o.svg"
                visible: itemsMenu.visible && newNoteItem.enabled
                onClicked: newNoteItem.trigger()
            }
            ToolButton {
                iconSource: "qrc:/FontAwesome/black/check-square-o.svg"
                visible: itemsMenu.visible && newTodoListItem.enabled
                onClicked: newImageItem.trigger()
            }
            ToolButton {
                iconSource: "qrc:/FontAwesome/black/image.svg"
                visible: itemsMenu.visible && newImageItem.enabled
                onClicked: newTodoListItem.trigger()
            }
            ToolButton {
                iconSource: "qrc:/FontAwesome/black/pencil-square-o.svg"
                menu: ColorMenu {
                    id: toolbarColorMenu
                    item: colorMenu.item
                }
                visible: menu.visible
            }
            Item {
                Layout.fillWidth: true
            }
            ToolButton {
                iconSource: "qrc:/FontAwesome/black/trash-o.svg"
                visible: stack.currentItem && typeof(stack.currentItem["deleteItem"]) === "function"
                onClicked: stack.currentItem.deleteItem()
            }
            ToolButton {
                iconSource: "qrc:/FontAwesome/black/check.svg"
                visible: stack.currentItem && typeof(stack.currentItem["save"]) === "function"
                onClicked: Logic.saveCurrent(stack)
            }
        }
    }
    
    Component.onCompleted: {
        Globals.appWindow = window;
        window.width = App.loadValue("width", width);
        window.height = App.loadValue("height", height);
        if (App.loadValue("maximized", false)) {
            window.visibility = Window.Maximized
        }
    }
    
    onClosing: {
        if (Qt.platform.os == "android") {
            if (stack.currentItem) {
                Logic.cancelCurrent(stack);
                close.accepted = false;
                return;
            }
        }
        App.saveValue("width", width);
        App.saveValue("height", height);
        App.saveValue("maximized", visibility === Window.Maximized);
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
        Keys.onEscapePressed: Logic.goBack(stack, event)
        Keys.onBackPressed: Logic.goBack(stack, event)
        
        FileDialog {
            id: openLocalLibraryDialog
            title: qsTr("Open Local Library")
            selectFolder: true
            folder: shortcuts.home
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
            onLibraryClicked: Logic.viewLibrary(stack, currentLibrary, libraryPage)
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
