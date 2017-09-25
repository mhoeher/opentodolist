import QtQuick 2.5
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.1
import QtQuick.Window 2.3


import OpenTodoList 1.0
import OpenTodoList.UI 1.0

import "MainWindowLogic.js" as Logic

ApplicationWindow {
    id: window

    signal openLocalLibrary()

    function focus() {
        rootItem.forceActiveFocus();
    }

    property Item helpPage: null

    title: qsTr("OpenTodoList") + " - " + applicationVersion
    visible: true
    width: 640
    height: 480

    header: ToolBar {
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
                    id: sidebarControl
                    symbol: Fonts.symbols.faBars
                    anchors.verticalCenter: parent.verticalCenter
                    visible: leftSideBar.compact && stackView.depth <= 1
                    checked: dynamicLeftDrawer.visible
                    onClicked: dynamicLeftDrawer.visible = !dynamicLeftDrawer.visible
                }
                Symbol {
                    symbol: Fonts.symbols.faArrowLeft
                    visible: stackView.canGoBack
                    onClicked: stackView.pop()
                    anchors.verticalCenter: parent.verticalCenter
                }
                Symbol {
                    symbol: Fonts.symbols.faStickyNoteO
                    visible: stackView.currentItem && typeof(stackView.currentItem["newNote"]) === "function"
                    //shortcut: qsTr("Ctrl+Shift+N")
                    onClicked: stackView.currentItem.newNote()
                    anchors.verticalCenter: parent.verticalCenter
                }
                Symbol {
                    symbol: Fonts.symbols.faCheckSquareO
                    visible: stackView.currentItem && typeof(stackView.currentItem["newTodoList"]) === "function"
                    onClicked: stackView.currentItem.newTodoList()
                    anchors.verticalCenter: parent.verticalCenter
                }
                Symbol {
                    symbol: Fonts.symbols.faPictureO
                    visible: stackView.currentItem && typeof(stackView.currentItem["newImage"]) === "function"
                    onClicked: stackView.currentItem.newImage()
                    anchors.verticalCenter: parent.verticalCenter
                }
                Symbol {
                    symbol: Fonts.symbols.faPencilSquareO
                    menu: ColorMenu {
                        item: stackView.currentItem && stackView.currentItem["item"] ? stackView.currentItem.item : null
                    }
                    anchors.verticalCenter: parent.verticalCenter
                    visible: menu.item !== null
                }
                Symbol {
                    id: styleSelector
                    visible: formatMenu.formatting
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
                    id: makeBoldToolButton
                    symbol: Fonts.symbols.faBold
                    visible: formatMenu.formatting && formatMenu.boldItem.enabled
                    checked: formatMenu.boldItem.checked
                    anchors.verticalCenter: parent.verticalCenter
                    onClicked: formatMenu.boldItem.triggered()
                }
                Symbol {
                    id: makeItalicToolButton
                    symbol: Fonts.symbols.faItalic
                    visible: formatMenu.formatting && formatMenu.italicItem.enabled
                    checked: formatMenu.italicItem.checked
                    anchors.verticalCenter: parent.verticalCenter
                    onClicked: formatMenu.italicItem.triggered()
                }
                Symbol {
                    id: makeUnderlineToolButton
                    symbol: Fonts.symbols.faUnderline
                    visible: formatMenu.formatting&& formatMenu.underlineItem.enabled
                    checked: formatMenu.underlineItem.checked
                    anchors.verticalCenter: parent.verticalCenter
                    onClicked: formatMenu.underlineItem.triggered()
                }
                Symbol {
                    symbol: Fonts.symbols.faStrikethrough
                    visible: formatMenu.formatting&& formatMenu.strikethroughItem.enabled
                    checked: formatMenu.strikethroughItem.checked
                    anchors.verticalCenter: parent.verticalCenter
                    onClicked: formatMenu.strikethroughItem.triggered()
                }
                Symbol {
                    id: makeUnorderedListToolButton
                    symbol: Fonts.symbols.faList
                    visible: formatMenu.formatting
                    checked: formatMenu.bulletListItem.checked
                    anchors.verticalCenter: parent.verticalCenter
                    onClicked: formatMenu.bulletListItem.triggered()
                }
                Symbol {
                    id: makeOrderedListToolButton
                    symbol: Fonts.symbols.faListOl
                    visible: formatMenu.formatting
                    checked: formatMenu.orderedListItem.checked
                    anchors.verticalCenter: parent.verticalCenter
                    onClicked: formatMenu.orderedListItem.triggered()
                }
                Symbol {
                    id: indentToolButton
                    symbol: Fonts.symbols.faIndent
                    visible: formatMenu.formatting
                    anchors.verticalCenter: parent.verticalCenter
                    onClicked: formatMenu.indentItem.triggered()
                }
                Symbol {
                    id: outdentToolButton
                    symbol: Fonts.symbols.faOutdent
                    visible: formatMenu.formatting
                    anchors.verticalCenter: parent.verticalCenter
                    onClicked: formatMenu.outdentItem.triggered()
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
                    visible: formatMenu.formatting
                    anchors.verticalCenter: parent.verticalCenter
                    menu: Menu {
                        MenuItem {
                            text: formatMenu.alignLeftItem.text
                            checkable: true
                            checked: formatMenu.alignLeftItem.checked
                            onTriggered: formatMenu.alignLeftItem.triggered()
                        }
                        MenuItem {
                            text: formatMenu.alignCenterItem.text
                            checkable: true
                            checked: formatMenu.alignCenterItem.checked
                            onTriggered: formatMenu.alignCenterItem.triggered()
                        }
                        MenuItem {
                            text: formatMenu.alignRightItem.text
                            checkable: true
                            checked: formatMenu.alignRightItem.checked
                            onTriggered: formatMenu.alignRightItem.triggered()
                        }
                        MenuItem {
                            text: formatMenu.alignJustifyItem.text
                            checkable: true
                            checked: formatMenu.alignJustifyItem.checked
                            onTriggered: formatMenu.alignJustifyItem.triggered()
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
                id: searchToolButton

                symbol: Fonts.symbols.faSearch
                enabled: stackView.currentItem && (typeof(stackView.currentItem["find"]) === "function")
                onClicked: stackView.currentItem.find()
                anchors.verticalCenter: parent.verticalCenter
            }
            Symbol {
                symbol: Fonts.symbols.faCopy
                visible: activeFocusItem && typeof(activeFocusItem["copy"]) === "function"
                onClicked: activeFocusItem.copy()
                anchors.verticalCenter: parent.verticalCenter
            }
            Symbol {
                symbol: Fonts.symbols.faCut
                visible: activeFocusItem && typeof(activeFocusItem["cut"]) === "function"
                onClicked: activeFocusItem.cut()
                anchors.verticalCenter: parent.verticalCenter
            }
            Symbol {
                symbol: Fonts.symbols.faPaste
                visible: activeFocusItem && typeof(activeFocusItem["paste"]) === "function"
                onClicked: activeFocusItem.paste()
                anchors.verticalCenter: parent.verticalCenter
            }
            BusyIndicator {
                visible: stackView.isBusy
                anchors.verticalCenter: parent.verticalCenter
                height: parent.height
                width: height
            }
            Symbol {
                symbol: Fonts.symbols.faTrashO
                visible: stackView.currentItem && typeof(stackView.currentItem["deleteItem"]) === "function"
                anchors.verticalCenter: parent.verticalCenter
                onClicked: stackView.currentItem.deleteItem()
            }
            Symbol {
                symbol: Fonts.symbols.faCheck
                visible: stackView.currentItem && typeof(stackView.currentItem["save"]) === "function"
                anchors.verticalCenter: parent.verticalCenter
                onClicked: Logic.saveCurrent(stackView)
            }
            Symbol {
                symbol: Fonts.symbols.faEllipsisV
                visible: stackView.hasPageMenu
                anchors.verticalCenter: parent.verticalCenter
                onClicked: stackView.currentItem.pageMenu.open()
            }
        }
    }

    Shortcut {
        sequence: StandardKey.Quit
        onActivated: Qt.quit()
    }

    Shortcut {
        sequence: StandardKey.Find
        onActivated: searchToolButton.clicked(null)
    }

    Shortcut {
        sequence: StandardKey.Back
        onActivated: if (stackView.canGoBack) { stackView.pop(); }
    }

    Shortcut {
        sequence: qsTr("Ctrl+Shift+L")
        onActivated: if (leftSideBar.compact) {
                         dynamicLeftDrawer.visible = !dynamicLeftDrawer.visible;
                     }
    }

    Shortcut {
        sequence: StandardKey.Bold
        onActivated: makeBoldToolButton.clicked(null)
    }

    Shortcut {
        sequence: StandardKey.Italic
        onActivated: makeItalicToolButton.clicked(null)
    }

    Shortcut {
        sequence: StandardKey.Underline
        onActivated: makeUnderlineToolButton.clicked(null)
    }

    Shortcut {
        sequence: qsTr("Ctrl+Shift+U")
        onActivated: makeUnorderedListToolButton.clicked(null)
    }

    Shortcut {
        sequence: qsTr("Ctrl+Shift+O")
        onActivated: makeOrderedListToolButton.clicked(null)
    }

    Shortcut {
        sequence: qsTr("Ctrl+Tab")
        onActivated: indentToolButton.clicked(null)
    }

    Shortcut {
        sequence: qsTr("Ctrl+Shift+Tab")
        onActivated: outdentToolButton.clicked(null)
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
        if (Qt.os === "android") {
            if (leftSideBar.compact && leftSideBar.visible) {
                leftSideBar.visible = false;
                close.accepted = false;
                return;
            } else if (stackView.depth > 1) {
                Logic.cancelCurrent(stackView);
                close.accepted = false;
                console.error("Handled close request.");
                return;
            }
        }
        close.accepted = true;
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


    LibrariesSideBar {
        id: leftSideBar
        helpVisible: helpPage !== null
        anchors.fill: parent
        compact: applicationWindow.width <= Globals.fontPixelSize * 60
        onCurrentLibraryChanged: Logic.viewLibrary(stackView, currentLibrary, currentTag, libraryPage)
        onCurrentTagChanged: Logic.viewLibrary(stackView, currentLibrary, currentTag, libraryPage)
        onNewLibrary: {
            stackView.clear();
            stackView.push(newSyncedLibraryPage);
        }
        onAboutPageRequested: {
            if (helpPage) {
                stackView.pop(helpPage);
            } else {
                leftSideBar.currentLibrary = null;
                leftSideBar.currentTag = "";
                helpPage = stackView.push(aboutPage, {
                                              stack: stackView,
                                              onClosed: function() { helpPage = null; }
                                          });
            }
        }
        parent: compact ? dynamicLeftDrawer.contentItem : staticLeftSideBar
        onClose: dynamicLeftDrawer.close()
    }

    Item {
        id: staticLeftSideBar
        width: leftSideBar.compact ? 0 : 15 * Globals.fontPixelSize
        height: parent.height
    }

    Drawer {
        id: dynamicLeftDrawer
        edge: Qt.LeftEdge
        width: 15 * Globals.fontPixelSize
        height: window.height - window.header.height
        y: window.header.height
    }

    FocusScope {
        id: rootItem

        focus: true
        anchors {
            top: parent.top
            right: parent.right
            bottom: parent.bottom
            left: staticLeftSideBar.right
        }

        Image {
            anchors.fill: parent
            source: "cork_1/cork_1.png"
            fillMode: Image.Tile
        }

        StackView {
            id: stackView

            property bool hasSync: !!currentItem && (typeof(currentItem.sync) === "function")
            property bool syncRunning: !!currentItem && !!currentItem.syncRunning
            property bool isBusy: syncRunning
            property bool hasPageMenu: !!currentItem && !!currentItem.pageMenu
            property bool canGoBack: currentItem !== null && depth > 1

            anchors.fill: parent
            clip: true
        }

        Connections {
            target: stackView.currentItem
            ignoreUnknownSignals: true
            onClosePage: stackView.pop()
            onOpenPage: stackView.push(component, properties)
            onClearAndOpenPage: {
                stackView.clear();
                stackView.push(component, properties);
            }
        }

        MouseArea {
            anchors.fill: stackView
            enabled: stackView.busy
        }

        Rectangle {
            id: updateNotificationBar

            property bool shown: App.updatesAvailable && App.hasUpdateService

            anchors {
                left: parent.left
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
                    stackView.push(Qt.resolvedUrl(item.itemType + "Page.qml"),
                                   { item: item, library: library, stack: stackView });
                }
            }
        }

        Component {
            id: aboutPage
            AboutPage {}
        }

        Component {
            id: newSyncedLibraryPage

            SynchronizerBackendSelectionPage {
                onCancelled: {
                    Logic.viewLibrary(stackView,
                                      leftSideBar.currentLibrary,
                                      leftSideBar.currentTag,
                                      libraryPage);
                }
                onBackendSelected: {
                    switch (synchronizer.synchronizer) {
                    case "WebDAVSynchronizer":
                        stackView.replace(webDavConnectionSetupPage,
                                          {"synchronizer": synchronizer});
                        break;
                    case "":
                        stackView.replace(newLocalLibraryPage);
                        break;
                    }
                }
            }
        }

    }

    Component {
        id: webDavConnectionSetupPage

        WebDAVConnectionSettingsPage {
            onCancelled: {
                Logic.viewLibrary(stackView,
                                  leftSideBar.currentLibrary,
                                  leftSideBar.currentTag,
                                  libraryPage);
            }
            onConnectionDataAvailable: {
                stackView.replace(
                            existingLibrarySelectionPage,
                            {"synchronizer": synchronizer});
            }
        }
    }

    Component {
        id: existingLibrarySelectionPage

        SyncLibrarySelectionPage {
            onCancelled: {
                Logic.viewLibrary(stackView,
                                  leftSideBar.currentLibrary,
                                  leftSideBar.currentTag,
                                  libraryPage);
            }
            onLibraryAvailable: {
                stackView.replace(
                            newLocalLibraryPage,
                            {"synchronizer": synchronizer});
            }
        }
    }

    Component {
        id: newLocalLibraryPage

        NewLibraryPage {
            onCancelled: {
                Logic.viewLibrary(stackView,
                                  leftSideBar.currentLibrary,
                                  leftSideBar.currentTag,
                                  libraryPage);
            }
            onLibraryAvailable: {
                var lib = App.addLibrary(synchronizer);
                if (lib !== null) {
                    leftSideBar.currentLibrary = lib;
                } else {
                    console.error("Failed to create library!");
                    Logic.viewLibrary(stackView,
                                      leftSideBar.currentLibrary,
                                      leftSideBar.currentTag,
                                      libraryPage);
                }
            }
        }
    }

    Connections {
        target: !!application.messageReceived ? application : null
        onMessageReceived: {
            window.show();
            window.raise();
        }
    }
}
