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
    
    title: qsTr("OpenTodoList") + " - " + applicationVersion
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
            title: qsTr("Navigate")
            MenuItem {
                text: qsTr("Back")
                enabled: stack.depth > 1
                shortcut: StandardKey.Back
                onTriggered: Logic.goBack(stack)
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
            MenuItem {
                id: newTaskItem
                text: qsTr("New T&ask");
                shortcut: qsTr("Ctrl+Shift+A")
                enabled: stack.currentItem && typeof(stack.currentItem["newTask"]) === "function"
                onTriggered: stack.currentItem.newTask()
            }
            MenuSeparator {}
            MenuItem {
                property Item item: Logic.parentItemHavingFunction(activeFocusItem, 
                                                                   "createItemAndOpen")
                text: qsTr("&Create Item")
                enabled: item
                shortcut: qsTr("Return")
                onTriggered: item.createItem()
                
            }
            MenuItem {
                property Item item: Logic.parentItemHavingFunction(activeFocusItem, "createItem")
                text: qsTr("C&reate Item and Open")
                enabled: item
                shortcut: qsTr("Ctrl+Return")
                onTriggered: item.createItemAndOpen()
            }
        }
        
        Menu {
            id: editMenu
            
            title: qsTr("&Edit")
            
            ColorMenu {
                id: colorMenu
                item: stack.currentItem && stack.currentItem["item"] ? stack.currentItem.item : null
            }
            
            MenuItem {
                id: copyItem
                text: qsTr("&Copy")
                enabled: activeFocusItem && typeof(activeFocusItem["copy"]) === "function"
                shortcut: StandardKey.Copy
                onTriggered: activeFocusItem.copy()
            }
            MenuItem {
                id: cutItem
                text: qsTr("C&ut")
                enabled: activeFocusItem && typeof(activeFocusItem["cut"]) === "function"
                shortcut: StandardKey.Cut
                onTriggered: activeFocusItem.cut()
            }
            MenuItem {
                id: pasteItem
                text: qsTr("&Paste")
                enabled: activeFocusItem && typeof(activeFocusItem["paste"]) === "function"
                shortcut: StandardKey.Paste
                onTriggered: activeFocusItem.paste()
            }
        }
        
        RichTextEditorFormatMenu {
            id: formatMenu
            onParagraphStyleChanged: {
                for (var i = 0; i < styleComboBox.count; ++i) {
                    if (styleComboBox.get(i).style === paragraphStyle) {
                        styleComboBox.currentIndex = i;
                        return;
                    }
                }
                styleComboBox.currentIndex = 0;
            }
        }
        
        Menu {
            title: qsTr("Help")
            
            MenuItem {
                text: qsTr("Check for updates...")
                visible: App.hasUpdateService
                onTriggered: App.runUpdate()
            }
            MenuSeparator {}
            MenuItem {
                text: qsTr("About")
                shortcut: StandardKey.HelpContents
                onTriggered: Logic.viewPage(stack, aboutPage)
            }
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

        Flickable {
            anchors {
                left: parent.left
                right: toolBarRightButtonGroup.left
                rightMargin: Globals.defaultMargin
            }
            contentWidth: toolBarLeftButtonGroup.width
            height: parent.height
            clip: true

            Row {
                id: toolBarLeftButtonGroup
                height: parent.height
    
                Symbol {
                    symbol: Fonts.symbols.faAngleLeft
                    visible: stack.currentItem && typeof(stack.currentItem["cancel"]) === "function"
                    anchors.verticalCenter: parent.verticalCenter
                    onClicked: Logic.cancelCurrent(stack)
                }
                ComboBox {
                    id: libraryNavigator
                    model: App.libraries
                    editable: false
                    textRole: "name"
                    visible: !leftSideBar.visible && !formatMenu.visible
                    anchors.verticalCenter: parent.verticalCenter
                    onCurrentIndexChanged: leftSideBar.currentLibrary = App.libraries[currentIndex]
                }
                Symbol {
                    symbol: Fonts.symbols.faFolderOpenO
                    visible: addLocalLibraryItem.enabled
                    anchors.verticalCenter: parent.verticalCenter
                    onClicked: addLocalLibraryItem.trigger()
                }
                Symbol {
                    symbol: Fonts.symbols.faStickyNoteO
                    visible: itemsMenu.visible && newNoteItem.enabled
                    anchors.verticalCenter: parent.verticalCenter
                    onClicked: newNoteItem.trigger()
                }
                Symbol {
                    symbol: Fonts.symbols.faCheckSquareO
                    visible: itemsMenu.visible && newTodoListItem.enabled
                    anchors.verticalCenter: parent.verticalCenter
                    onClicked: newTodoListItem.trigger()
                }
                Symbol {
                    symbol: Fonts.symbols.faCheckSquareO
                    visible: itemsMenu.visible && newTodoItem.enabled
                    anchors.verticalCenter: parent.verticalCenter
                    onClicked: newTodoItem.trigger()
                }
                Symbol {
                    symbol: Fonts.symbols.faCheckSquareO
                    visible: itemsMenu.visible && newTaskItem.enabled
                    anchors.verticalCenter: parent.verticalCenter
                    onClicked: newTaskItem.trigger()
                }
                Symbol {
                    symbol: Fonts.symbols.faPictureO
                    visible: itemsMenu.visible && newImageItem.enabled
                    anchors.verticalCenter: parent.verticalCenter
                    onClicked: newImageItem.trigger()
                }
                Symbol {
                    symbol: Fonts.symbols.faPencilSquareO
                    menu: ColorMenu {
                        visible: colorMenu.visible
                        item: colorMenu.item
                    }
                    anchors.verticalCenter: parent.verticalCenter
                    visible: menu.visible
                }
                Symbol {
                    id: styleSelector
                    visible: formatMenu.visible
                    anchors.verticalCenter: parent.verticalCenter
                    symbol: Fonts.symbols.faEyeDropper
                    menu: Menu {
                        MenuItem {
                            text: "Default"
                            onTriggered: formatMenu.applyParagraphStyle(DocumentFormatter.Default)
                        }
                        MenuItem {
                            text: "Title 1"
                            onTriggered: formatMenu.applyParagraphStyle(DocumentFormatter.H1)
                        }
                        MenuItem {
                            text: "Title 2"
                            onTriggered: formatMenu.applyParagraphStyle(DocumentFormatter.H2)
                        }
                        MenuItem {
                            text: "Title 3"
                            onTriggered: formatMenu.applyParagraphStyle(DocumentFormatter.H3)
                        }
                        MenuItem {
                            text: "Title 4"
                            onTriggered: formatMenu.applyParagraphStyle(DocumentFormatter.H4)
                        }
                        MenuItem {
                            text: "Title 5"
                            onTriggered: formatMenu.applyParagraphStyle(DocumentFormatter.H5)
                        }
                        MenuItem {
                            text: "Title 6"
                            onTriggered: formatMenu.applyParagraphStyle(DocumentFormatter.H6)
                        }
                        MenuItem {
                            text: "Code"
                            onTriggered: formatMenu.applyParagraphStyle(DocumentFormatter.Code)
                        }
                    }
                }
                Symbol {
                    symbol: Fonts.symbols.faBold
                    visible: formatMenu.visible && formatMenu.boldItem.enabled
                    checked: formatMenu.boldItem.checked
                    anchors.verticalCenter: parent.verticalCenter
                    onClicked: formatMenu.boldItem.trigger()
                }
                Symbol {
                    symbol: Fonts.symbols.faItalic
                    visible: formatMenu.visible && formatMenu.italicItem.enabled
                    checked: formatMenu.italicItem.checked
                    anchors.verticalCenter: parent.verticalCenter
                    onClicked: formatMenu.italicItem.trigger()
                }
                Symbol {
                    symbol: Fonts.symbols.faUnderline
                    visible: formatMenu.visible&& formatMenu.underlineItem.enabled
                    checked: formatMenu.underlineItem.checked
                    anchors.verticalCenter: parent.verticalCenter
                    onClicked: formatMenu.underlineItem.trigger()
                }
                Symbol {
                    symbol: Fonts.symbols.faStrikethrough
                    visible: formatMenu.visible&& formatMenu.strikethroughItem.enabled
                    checked: formatMenu.strikethroughItem.checked
                    anchors.verticalCenter: parent.verticalCenter
                    onClicked: formatMenu.strikethroughItem.trigger()
                }
                Symbol {
                    symbol: Fonts.symbols.faList
                    visible: formatMenu.visible
                    checked: formatMenu.bulletListItem.checked
                    anchors.verticalCenter: parent.verticalCenter
                    onClicked: formatMenu.bulletListItem.trigger()
                }
                Symbol {
                    symbol: Fonts.symbols.faListOl
                    visible: formatMenu.visible
                    checked: formatMenu.orderedListItem.checked
                    anchors.verticalCenter: parent.verticalCenter
                    onClicked: formatMenu.orderedListItem.trigger()
                }
                Symbol {
                    symbol: Fonts.symbols.faIndent
                    visible: formatMenu.visible
                    anchors.verticalCenter: parent.verticalCenter
                    onClicked: formatMenu.indentItem.trigger()
                }
                Symbol {
                    symbol: Fonts.symbols.faOutdent
                    visible: formatMenu.visible
                    anchors.verticalCenter: parent.verticalCenter
                    onClicked: formatMenu.outdentItem.trigger()
                }
                Symbol {
                    symbol: {
                        if (formatMenu.alignLeftItem.checked) {
                            return Fonts.symbols.faAlignLeft;
                        } else if (formatMenu.alignRightItem.checked) {
                            return Fonts.symbols.faAlignRight;
                        } else if (formatMenu.alignCenterItem.checked) {
                            return Fonts.symbols.faAlignCenter;
                        } else if (formatMenu.alignJustifyItem.checked) {
                            return Fonts.symbols.faAlignJustify;
                        } else {
                            return "";
                        }
                    }
                    visible: formatMenu.visible
                    anchors.verticalCenter: parent.verticalCenter
                    menu: Menu {
                        MenuItem {
                            text: formatMenu.alignLeftItem.text
                            checkable: true
                            checked: formatMenu.alignLeftItem.checked
                            onTriggered: formatMenu.alignLeftItem.trigger()
                        }
                        MenuItem {
                            text: formatMenu.alignCenterItem.text
                            checkable: true
                            checked: formatMenu.alignCenterItem.checked
                            onTriggered: formatMenu.alignCenterItem.trigger()
                        }
                        MenuItem {
                            text: formatMenu.alignRightItem.text
                            checkable: true
                            checked: formatMenu.alignRightItem.checked
                            onTriggered: formatMenu.alignRightItem.trigger()
                        }
                        MenuItem {
                            text: formatMenu.alignJustifyItem.text
                            checkable: true
                            checked: formatMenu.alignJustifyItem.checked
                            onTriggered: formatMenu.alignJustifyItem.trigger()
                        }
                    }
                }
            }
        }        
        
        Row {
            id: toolBarRightButtonGroup
            
            anchors {
                right: parent.right
            }
            height: parent.height
            
            Symbol {
                symbol: Fonts.symbols.faCopy
                visible: copyItem.enabled
                anchors.verticalCenter: parent.verticalCenter
                onClicked: copyItem.trigger()
            }
            Symbol {
                symbol: Fonts.symbols.faCut
                visible: cutItem.enabled
                anchors.verticalCenter: parent.verticalCenter
                onClicked: cutItem.trigger()
            }
            Symbol {
                symbol: Fonts.symbols.faPaste
                visible: pasteItem.enabled
                anchors.verticalCenter: parent.verticalCenter
                onClicked: pasteItem.trigger()
            }
            Symbol {
                symbol: Fonts.symbols.faTrashO
                visible: stack.currentItem && typeof(stack.currentItem["deleteItem"]) === "function"
                anchors.verticalCenter: parent.verticalCenter
                onClicked: stack.currentItem.deleteItem()
            }
            Symbol {
                symbol: Fonts.symbols.faCheck
                visible: stack.currentItem && typeof(stack.currentItem["save"]) === "function"
                anchors.verticalCenter: parent.verticalCenter
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
            if (stack.depth > 1) {
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
            width: 20 * Globals.fontPixelSize
            edge: Qt.LeftEdge
            x: applicationWindow.width > Globals.fontPixelSize * 60 ? 0 : -width
            visible: x > -width
            onCurrentLibraryChanged: Logic.viewLibrary(stack, currentLibrary, libraryPage)
        }
        
        StackView {
            id: stack
            anchors {
                left: leftSideBar.right
                top: parent.top
                right: parent.right
                bottom: updateNotificationBar.top
            }
            clip: true
        }
        
        MouseArea {
            anchors.fill: stack
            enabled: stack.busy
        }
        
        Rectangle {
            id: updateNotificationBar
            
            property bool shown: App.updatesAvailable && App.hasUpdateService
            
            anchors {
                left: leftSideBar.right
                right: parent.right
            }
            height: updateNotificationBarLayout.height + Globals.defaultMargin
            color: Colors.makeTransparent(Colors.primary, 0.3)
            y: shown ? parent.height - height : parent.height
            
            RowLayout {
                id: updateNotificationBarLayout
                anchors {
                    verticalCenter: parent.verticalCenter
                    left: parent.left
                    right: parent.right
                    margins: Globals.defaultMargin
                }
                
                Label {
                    text: qsTr( "A new version of OpenTodoList is available.")
                    Layout.fillWidth: true
                    wrapMode: Text.WrapAnywhere
                }
                Button {
                    text: qsTr("Later")
                    onClicked: updateNotificationBar.shown = false
                }
                Button {
                    text: qsTr("Update Now")
                    onClicked: {
                        App.runUpdate();
                        updateNotificationBar.shown = false;
                    }
                }
            }
            
            Connections {
                target: App
                onUpdatesAvailableChanged:
                    updateNotificationBar.shown = App.updatesAvailable && App.hasUpdateService
            }
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
        
        Component {
            id: aboutPage
            AboutPage {}
        }
    }
}
