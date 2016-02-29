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
                
                ComboBox {
                    id: libraryNavigator
                    model: App.libraries
                    editable: false
                    textRole: "name"
                    visible: !leftSideBar.visible
                    height: parent.height
                    anchors.verticalCenter: parent.verticalCenter
                    onCurrentIndexChanged: leftSideBar.currentLibrary = App.libraries[currentIndex]
                }
    
                Symbol {
                    symbol: Fonts.symbols.faAngleLeft
                    visible: stack.currentItem && typeof(stack.currentItem["cancel"]) === "function"
                    anchors.verticalCenter: parent.verticalCenter
                    onClicked: Logic.cancelCurrent(stack)
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
                ComboBox {
                    id: styleComboBox
                    visible: formatMenu.visible
                    anchors.verticalCenter: parent.verticalCenter
                    height: parent.height
                    model: ListModel {
                        ListElement {
                            label: ""
                            style: DocumentFormatter.UnknownParagraphStyle
                        }
                        ListElement {
                            label: "Default"
                            style: DocumentFormatter.Default
                        }
                        ListElement {
                            label: "Title 1"
                            style: DocumentFormatter.H1
                        }
                        ListElement {
                            label: "Title 2"
                            style: DocumentFormatter.H2
                        }
                        ListElement {
                            label: "Title 3"
                            style: DocumentFormatter.H3
                        }
                        ListElement {
                            label: "Title 4"
                            style: DocumentFormatter.H4
                        }
                        ListElement {
                            label: "Title 5"
                            style: DocumentFormatter.H5
                        }
                        ListElement {
                            label: "Title 6"
                            style: DocumentFormatter.H6
                        }
                        ListElement {
                            label: "Code"
                            style: DocumentFormatter.Code
                        }
                    }
                    textRole: "label"
                    onCurrentIndexChanged: {
                        if (visible) {
                            formatMenu.applyParagraphStyle(model.get(currentIndex).style);
                        }
                    }
                }
                Symbol {
                    symbol: Fonts.symbols.faBold
                    visible: formatMenu.visible
                    checked: formatMenu.boldItem.checked
                    anchors.verticalCenter: parent.verticalCenter
                    onClicked: formatMenu.boldItem.trigger()
                }
                Symbol {
                    symbol: Fonts.symbols.faItalic
                    visible: formatMenu.visible
                    checked: formatMenu.italicItem.checked
                    anchors.verticalCenter: parent.verticalCenter
                    onClicked: formatMenu.italicItem.trigger()
                }
                Symbol {
                    symbol: Fonts.symbols.faUnderline
                    visible: formatMenu.visible
                    checked: formatMenu.underlineItem.checked
                    anchors.verticalCenter: parent.verticalCenter
                    onClicked: formatMenu.underlineItem.trigger()
                }
                Symbol {
                    symbol: Fonts.symbols.faStrikethrough
                    visible: formatMenu.visible
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
                    symbol: Fonts.symbols.faAlignLeft
                    visible: formatMenu.visible
                    checked: formatMenu.alignLeftItem.checked
                    anchors.verticalCenter: parent.verticalCenter
                    onClicked: formatMenu.alignLeftItem.trigger()
                }
                Symbol {
                    symbol: Fonts.symbols.faAlignCenter
                    visible: formatMenu.visible
                    checked: formatMenu.alignCenterItem.checked
                    anchors.verticalCenter: parent.verticalCenter
                    onClicked: formatMenu.alignCenterItem.trigger()
                }
                Symbol {
                    symbol: Fonts.symbols.faAlignRight
                    visible: formatMenu.visible
                    checked: formatMenu.alignRightItem.checked
                    anchors.verticalCenter: parent.verticalCenter
                    onClicked: formatMenu.alignRightItem.trigger()
                }
                Symbol {
                    symbol: Fonts.symbols.faAlignJustify
                    visible: formatMenu.visible
                    checked: formatMenu.alignJustifyItem.checked
                    anchors.verticalCenter: parent.verticalCenter
                    onClicked: formatMenu.alignJustifyItem.trigger()
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
        
        Component {
            id: aboutPage
            AboutPage {}
        }
    }
}
