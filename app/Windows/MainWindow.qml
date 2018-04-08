import QtQuick 2.9
import QtQuick.Window 2.3
import QtQuick.Layouts 1.1

import "../Components"
import "../Fonts"
import "../Widgets"
import "../Menues"
import "../Pages"

import OpenTodoList 1.0 as OTL

ApplicationWindow {
    id: window

    property ItemCreatedNotification itemCreatedNotification: null

    title: qsTr("OpenTodoList") + " - " + applicationVersion
    visible: true
    width: 640
    height: 480

    //    signal openLocalLibrary()

    //    function focus() {
    //        rootItem.forceActiveFocus();
    //    }

    function viewLibrary(lib, tag, special) {
        lib = lib || librariesSideBar.currentLibrary;
        tag = tag || librariesSideBar.currentTag;
        special = special || librariesSideBar.specialView;
        stackView.clear();
        if (lib) {
            switch (special) {
            case "schedule":
                stackView.push(scheduleViewPage, { library: lib});
                break;
            default:
                stackView.push(libraryPage, { library: lib, tag: tag });
                break;
            }
            if (d.completed) {
                OTL.Application.saveValue("lastLibrary", lib.uid.toString());
                OTL.Application.saveValue("lastTag", tag);
                OTL.Application.saveValue("specialView", special);
            }
        }
    }

    property Item helpPage: null

    header: ToolBar {
        leftPadding: 10
        rightPadding: 10

        RowLayout {
            width: parent.width

            ToolButton {
                id: sidebarControl
                symbol: Icons.faBars
                anchors.verticalCenter: parent.verticalCenter
                visible: librariesSideBar.compact && stackView.depth <= 1
                checked: dynamicLeftDrawer.visible
                onClicked: dynamicLeftDrawer.visible = !dynamicLeftDrawer.visible
            }
            ToolButton {
                symbol: Icons.faArrowLeft
                visible: stackView.canGoBack
                onClicked: stackView.pop()
                anchors.verticalCenter: parent.verticalCenter
            }

            ToolButton {
                symbol: stackView.isChecked ? Icons.faCheckCircle :
                                              Icons.faCircle
                font.family: Fonts.icons
                visible: stackView.isCheckable
                onClicked: stackView.item.done = !stackView.item.done
            }

            Label {
                id: pageTitleLabel
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignVCenter

                elide: Text.ElideRight

                Binding {
                    target: pageTitleLabel
                    property: "text"
                    value: stackView.currentItem ? stackView.currentItem.title : ""
                }
            }

            ToolButton {
                symbol: Icons.faPencilAlt
                visible: stackView.currentItem && typeof(stackView.currentItem["renameItem"]) === "function"
                anchors.verticalCenter: parent.verticalCenter
                onClicked: stackView.currentItem.renameItem()
            }

            ToolButton {
                symbol: Icons.faPaintBrush
                menu: ColorMenu {
                    y: window.header.height
                    item: stackView.hasColor ? stackView.currentItem.item : null
                }
                anchors.verticalCenter: parent.verticalCenter
                visible: menu.item !== null
            }

            ToolButton {
                id: searchToolButton

                symbol: Icons.faSearch
                visible: stackView.currentItem && (typeof(stackView.currentItem["find"]) === "function")
                onClicked: stackView.currentItem.find()
                anchors.verticalCenter: parent.verticalCenter
            }

            ToolButton {
                symbol: Icons.faTrashAlt
                visible: stackView.currentItem && typeof(stackView.currentItem["deleteItem"]) === "function"
                anchors.verticalCenter: parent.verticalCenter
                onClicked: stackView.currentItem.deleteItem()
            }
            ToolButton {
                symbol: Icons.faEllipsisV
                visible: stackView.hasPageMenu
                anchors.verticalCenter: parent.verticalCenter
                onClicked: {
                    if (stackView.currentItem.pageMenu.visible) {
                        stackView.currentItem.pageMenu.close();
                    } else {
                        stackView.currentItem.pageMenu.open();
                    }
                }
            }
        }
    }

    QtObject {
        id: d

        property bool completed: false

        property string lastLibrary: ""
        property string lastTag: ""
        property string specialView: ""

        function reopenLastLibrary() {
            if (d.lastLibrary != "") {
                var libs = OTL.Application.libraries;
                for (var i = 0; i < libs.length; ++i) {
                    var lib = libs[i];
                    if (lib.uid.toString() === d.lastLibrary) {
                        librariesSideBar.currentLibrary = lib;
                        librariesSideBar.currentTag = d.lastTag;
                        librariesSideBar.specialView = d.specialView;
                        d.lastLibrary = "";
                        break;
                    }
                }
            }
        }
    }

    Action {
        id: newLibraryAction

        text: qsTr("New &Library")
        shortcut: StandardKey.New
        onTriggered: {
            stackView.clear();
            stackView.push(newSyncedLibraryPage);
        }
    }

    Action {
        id: newNoteAction

        text: qsTr("New &Note")
        onTriggered: stackView.currentItem.newNote()
        enabled: stackView.currentItem && typeof(stackView.currentItem.newNote) === "function"
    }

    Action {
        id: newTodoListAction

        text: qsTr("New &Todo List")
        onTriggered: stackView.currentItem.newTodoList()
        enabled: stackView.currentItem && typeof(stackView.currentItem.newTodoList) === "function"
    }

    Action {
        id: newImageAction

        text: qsTr("New &Image")
        onTriggered: stackView.currentItem.newImage()
        enabled: stackView.currentItem && typeof(stackView.currentItem.newImage) === "function"
    }

    Action {
        id: quitAction

        text: qsTr("&Quit")
        shortcut: StandardKey.Quit
        onTriggered: Qt.quit()
    }

    Action {
        id: findAction

        text: qsTr("&Find")
        shortcut: StandardKey.Find
        onTriggered: searchToolButton.clicked(null)
    }

    Action {
        id: goBackAction

        text: qsTr("Go &Back")
        shortcut: StandardKey.Back
        onTriggered: if (stackView.canGoBack) { stackView.pop(); }
    }

    Action {
        id: openLastCreatedItemAction

        shortcut: StandardKey.Open
        text: qsTr("Open Last &Created Item")
        enabled: !!window.itemCreatedNotification
        onTriggered: Globals.itemCreatedNotification.trigger()
    }

    Action {
        id: openLeftSideBarAction

        text: qsTr("Open &Left Side Bar")
        onTriggered: if (librariesSideBar.compact) {
                         dynamicLeftDrawer.visible = !dynamicLeftDrawer.visible;
                     }
    }

    Component.onCompleted: {
        width = OTL.Application.loadValue("width", width);
        height = OTL.Application.loadValue("height", height);
        d.lastLibrary = OTL.Application.loadValue("lastLibrary", "");
        d.lastTag = OTL.Application.loadValue("lastTag", "");
        d.specialView = OTL.Application.loadValue("specialView", "");
        if (OTL.Application.loadValue("maximized", "false") === "true") {
            window.visibility = Window.Maximized;
        }
        onVisibilityChanged.connect(function() {
            OTL.Application.saveValue(
                        "maximized", visibility === Window.Maximized);
        });
        onWidthChanged.connect(function() {
            if (visibility === Window.Windowed) {
                OTL.Application.saveValue("width", width);
            }
        });
        onHeightChanged.connect(function() {
            if (visibility === Window.Windowed) {
                OTL.Application.saveValue("height", height);
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
        id: librariesSideBar

        function viewSelectedLibrary() {
            window.viewLibrary(currentLibrary, currentTag, specialView);
        }

        helpVisible: helpPage !== null
        anchors.fill: parent
        compact: window.width < 600
        onCurrentLibraryChanged: viewSelectedLibrary()
        onCurrentTagChanged: viewSelectedLibrary()
        onSpecialViewChanged: viewSelectedLibrary()
        onNewLibrary: {
            stackView.clear();
            stackView.push(newSyncedLibraryPage);
        }
        onAboutPageRequested: {
            if (helpPage) {
                stackView.pop(helpPage);
            } else {
                librariesSideBar.currentLibrary = null;
                librariesSideBar.currentTag = "";
                helpPage = stackView.push(
                            aboutPage, {
                                stack: stackView,
                                onClosed: function() { helpPage = null; }
                            });
            }
        }
        parent: compact ? dynamicLeftDrawer.contentItem : staticLeftSideBar
        onClose: dynamicLeftDrawer.close()
    }

    Pane {
        anchors.fill: stackView
        visible: OTL.Application.libraries.length === 0

        BackgroundSymbol {
            symbol: Icons.faBook
        }

        BackgroundLabel {
            text: qsTr("Start by <a href='#newLibrary'>creating a new " +
                       "library</a>. Libraries are used to store " +
                       "different kinds of items like notes, todo lists " +
                       "and images.")
            onLinkActivated: if (link === "#newLibrary") {
                                 newLibraryAction.trigger();
                             }
        }
    }

    StackView {
        id: stackView

        property bool hasSync: !!currentItem &&
                               (typeof(currentItem.sync) === "function")
        property bool syncRunning: !!currentItem && !!currentItem.syncRunning
        property bool hasPageMenu: !!currentItem && !!currentItem.pageMenu
        property bool canGoBack: currentItem !== null && depth > 1

        property bool hasItem: currentItem &&
                               currentItem.item !== undefined
        property bool hasColor: hasItem && currentItem.item.color !== undefined
        property bool isCheckable: hasItem && currentItem.item.done !== undefined
        property bool isChecked: isCheckable ? currentItem.item.done : false

        anchors {
            left: staticLeftSideBar.right
            top: parent.top
            bottom: parent.bottom
            right: parent.right
        }

        clip: true
        visible: depth > 0
    }

    Pane {
        id: staticLeftSideBar
        width: librariesSideBar.compact ? 0 : Math.min(300, window.width / 3)
        height: parent.height
        elevation: 6
        visible: width > 0
    }

    Drawer {
        id: dynamicLeftDrawer
        edge: Qt.LeftEdge
        width: Math.min(300, window.width / 3)
        height: window.height - window.header.height
        y: window.header.height
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
        // Used to prevent interaction with the stack while we load
        // a page.
        anchors.fill: stackView
        enabled: stackView.busy
    }

    UpdateNotificationBar {}

    Component {
        id: libraryPage
        LibraryPage {
            onItemClicked: {
                stackView.push(Qt.resolvedUrl("../Pages/" +
                                              item.itemType + "Page.qml"),
                               { item: item, library: library, stack: stackView });
            }
        }
    }

    Component {
        id: scheduleViewPage
        ScheduleViewPage {}
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
                var lib = OTL.Application.addLibrary(synchronizer);
                if (lib !== null) {
                    librariesSideBar.currentLibrary = lib;
                    librariesSideBar.currentTag = "";
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

}
