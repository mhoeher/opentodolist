import QtQuick 2.9
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

    function viewLibrary(lib, tag) {
        lib = lib || leftSideBar.currentLibrary;
        tag = tag || leftSideBar.currentTag;
        stackView.clear();
        if (lib) {
            stackView.push(libraryPage, { library: lib, tag: tag });
            if (d.completed) {
                console.debug("Setting last library: " + lib.name + "@" + tag);
                App.saveValue("lastLibrary", lib.uid.toString());
                App.saveValue("lastTag", tag);
            }
        }
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
                symbol: Fonts.symbols.faTrashO
                visible: stackView.currentItem && typeof(stackView.currentItem["deleteItem"]) === "function"
                anchors.verticalCenter: parent.verticalCenter
                onClicked: stackView.currentItem.deleteItem()
            }
            Symbol {
                symbol: Fonts.symbols.faEllipsisV
                visible: stackView.hasPageMenu
                anchors.verticalCenter: parent.verticalCenter
                onClicked: stackView.currentItem.pageMenu.open()
            }
        }
    }

    QtObject {
        id: d

        property bool completed: false

        property string lastLibrary: ""
        property string lastTag: ""

        function reopenLastLibrary() {
            if (d.lastLibrary != "") {
                var libs = App.libraries;
                for (var i = 0; i < libs.length; ++i) {
                    var lib = libs[i];
                    console.debug("" + lib.uid.toString() + " == " + d.lastLibrary + "?");
                    if (lib.uid.toString() === d.lastLibrary) {
                        leftSideBar.currentLibrary = lib;
                        leftSideBar.currentTag = d.lastTag;
                        d.lastLibrary = "";
                        break;
                    }
                }
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
        d.lastLibrary = App.loadValue("lastLibrary", "");
        d.lastTag = App.loadValue("lastTag", "");
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
        d.reopenLastLibrary();
        d.completed = true;
    }

    onClosing: {
        if (Qt.platform.os === "android") {
            if (stackView.depth > 1) {
                stackView.pop();
                close.accepted = false;
                return;
            }
        }
        close.accepted = true;
    }


    LibrariesSideBar {
        id: leftSideBar
        helpVisible: helpPage !== null
        anchors.fill: parent
        compact: applicationWindow.width <= Globals.fontPixelSize * 60
        onCurrentLibraryChanged: window.viewLibrary(currentLibrary, currentTag)
        onCurrentTagChanged: window.viewLibrary(currentLibrary, currentTag)
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

        StackView {
            id: stackView

            property bool hasSync: !!currentItem && (typeof(currentItem.sync) === "function")
            property bool syncRunning: !!currentItem && !!currentItem.syncRunning
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

        UpdateNotificationBar {}

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
                onCancelled: window.viewLibrary()
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
            onCancelled: window.viewLibrary()
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
            onCancelled: window.viewLibrary()
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
            onCancelled: window.viewLibrary()
            onLibraryAvailable: {
                var lib = App.addLibrary(synchronizer);
                if (lib !== null) {
                    leftSideBar.currentLibrary = lib;
                } else {
                    console.error("Failed to create library!");
                    window.viewLibrary();
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

    SystemPalette {
        id: p
    }
}
