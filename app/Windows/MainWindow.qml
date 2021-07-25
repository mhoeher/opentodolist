import QtQuick 2.10
import QtQuick.Window 2.3
import QtQuick.Layouts 1.1
import QtQuick.Controls.Material 2.12
import Qt.labs.qmlmodels 1.0
import Qt.labs.settings 1.0

import "../Components"
import "../Controls" as C
import "../Fonts"
import "../Widgets"
import "../Menues"
import "../Pages"
import "../Utils"

import OpenTodoList 1.0 as OTL

C.ApplicationWindow {
    id: window

    property ItemCreatedNotification itemCreatedNotification: null
    property int defaultFontSize: 12

    title: qsTr("OpenTodoList") + " - " + applicationVersion
    visible: true
    width: 640
    height: 480

    // Sync all libraries whenever we show the main window. This ensures that users get an up
    // to date view when e.g. restoring from the system tray or opening the GUI on Android.
    onVisibleChanged: if (visible) { OTL.Application.syncAllLibraries(); }

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
    property Item settingsPage: null
    property Item accountsPage: null

    header: ApplicationToolBar {
        id: applicationToolBar

        sidebarControl.visible: librariesSideBar.compact && stackView.depth <= 1
        sidebarControl.checked: dynamicLeftDrawer.visible
        sidebarControl.onClicked: dynamicLeftDrawer.visible = !dynamicLeftDrawer.visible

        backToolButton.visible: OTL.Application.problemManager.problems.length > 0
        backToolButton.onClicked: stackView.replace(problemsPage)

        problemsButton.visible: OTL.Application.problemManager.problems.length > 0
        problemsButton.onClicked: stackView.replace(problemsPage)

        busyIndicator.visible: stackView.syncRunning

        pageActions: {
            if (stackView.currentItem && stackView.currentItem.pageActions) {
                return stackView.currentItem.pageActions;
            } else {
                return [];
            }
        }
        dynamicPageActions: d.dynamicPageActions

        Binding {
            target: applicationToolBar
            property: "title"
            value: stackView.currentItem ? stackView.currentItem.title : ""
        }
    }

    QtObject {
        id: d

        property bool completed: false

        // The list of all available page actions
        property list<ToolBarAction> dynamicPageActions

        property QuickNoteWindow quickNoteWindow: null

        function createQuickNoteEditor() {
            if (!AppSettings.supportsQuickEditor) {
                return;
            }

            let componentUrl = Qt.resolvedUrl("./QuickNoteWindow.qml");
            let component = Qt.createComponent(componentUrl);
            quickNoteWindow = component.createObject();
        }

        function showMainWindow() {
            if (!window.visible) {
                window.show();
            }
            window.requestActivate();
            window.raise();
        }

        function hideMainWindow() {
            window.hide();
        }

        function showQuickNotesEditor() {
            quickNoteWindow.show();
            quickNoteWindow.requestActivate();
            quickNoteWindow.raise();
        }

        dynamicPageActions: [
            ToolBarAction {
                symbol: Icons.mdiUndo
                text: qsTr("Undo")
                visible: undoAction.enabled
                onTriggered: undoAction.trigger()
            },

            ToolBarAction {
                symbol: Icons.mdiEdit
                text: qsTr("Rename")
                visible: {
                    return stackView.currentItem &&
                            typeof(stackView.currentItem["renameItem"]) ===
                            "function";
                }
                onTriggered: stackView.currentItem.renameItem()
            },

            ToolBarAction {
                symbol: Icons.mdiContentCut
                text: qsTr("Move")
                visible: {
                    return stackView.currentItem &&
                            typeof(stackView.currentItem["moveItem"]) ===
                            "function";
                }
                onTriggered: stackView.currentItem.moveItem()
            },

            ToolBarAction {
                symbol: Icons.mdiContentCopy
                text: qsTr("Copy")
                visible: {
                    return stackView.currentItem &&
                            typeof(stackView.currentItem["copyItem"]) ===
                            "function";
                }
                onTriggered: stackView.currentItem.copyItem()
            },

            ToolBarAction {
                symbol: Icons.mdiPalette
                text: qsTr("Color")
                menu: ColorMenu {
                    anchors.centerIn: parent
                    item: stackView.hasColor ? stackView.currentItem.item : null
                    parent: window.contentItem
                }
                visible: menu.item !== null
            },

            ToolBarAction {
                symbol: Icons.mdiLabel
                text: qsTr("Add Tag")
                visible: stackView.currentItem && (typeof(stackView.currentItem["addTag"]) === "function")
                onTriggered: stackView.currentItem.addTag()
            },

            ToolBarAction {
                symbol: Icons.mdiAttachment
                text: qsTr("Attach File")
                visible: stackView.currentItem && (typeof(stackView.currentItem["attach"]) === "function")
                onTriggered: stackView.currentItem.attach()
            },

            ToolBarAction {
                id: searchToolButtonAction

                symbol: Icons.mdiSearch
                text: qsTr("Search")
                visible: stackView.currentItem && (typeof(stackView.currentItem["find"]) === "function")
                onTriggered: stackView.currentItem.find()
            },

            ToolBarAction {
                symbol: Icons.mdiSort
                text: qsTr("Sort")
                visible: stackView.currentItem &&
                         typeof(stackView.currentItem.sort) === "function"
                onTriggered: stackView.currentItem.sort()
            },

            ToolBarAction {
                symbol: Icons.mdiCalendarToday
                text: qsTr("Set Due Date")
                visible: stackView.currentItem &&
                         typeof(stackView.currentItem.setDueDate) === "function"
                onTriggered: stackView.currentItem.setDueDate()
            },

            ToolBarAction {
                symbol: Icons.mdiDelete
                text: qsTr("Delete")
                visible: stackView.currentItem && typeof(stackView.currentItem["deleteItem"]) === "function"
                onTriggered: stackView.currentItem.deleteItem()
            },

            ToolBarAction {
                symbol: Icons.mdiRemoveDone
                text: qsTr("Delete Completed Items")
                visible: stackView.currentItem && typeof(stackView.currentItem["deleteCompletedItems"]) === "function"
                onTriggered: stackView.currentItem.deleteCompletedItems()
            },

            ToolBarAction {
                symbol: Icons.mdiPublishedWithChanges
                text: qsTr("Set Progress")
                visible: stackView.currentItem && typeof(stackView.currentItem["setProgress"]) === "function"
                onTriggered: stackView.currentItem.setProgress()
            }
        ]


    }

    C.Action {
        text: qsTr("Settings")
        shortcut: qsTr("Ctrl+,")
        onTriggered: librariesSideBar.showSettings()
    }

    C.Action {
        id: newLibraryAction

        text: qsTr("New &Library")
        shortcut: StandardKey.New
        onTriggered: {
            stackView.clear();
            stackView.push(newLibraryPage);
        }
    }

    C.Action {
        id: newNoteAction

        text: qsTr("New &Note")
        onTriggered: stackView.currentItem.newNote()
        enabled: stackView.currentItem && typeof(stackView.currentItem.newNote) === "function"
    }

    C.Action {
        id: newTodoListAction

        text: qsTr("New &Todo List")
        onTriggered: stackView.currentItem.newTodoList()
        enabled: stackView.currentItem && typeof(stackView.currentItem.newTodoList) === "function"
    }

    C.Action {
        id: newImageAction

        text: qsTr("New &Image")
        onTriggered: stackView.currentItem.newImage()
        enabled: stackView.currentItem && typeof(stackView.currentItem.newImage) === "function"
    }

    C.Action {
        id: quitAction

        text: qsTr("&Quit")
        shortcut: StandardKey.Quit
        onTriggered: Qt.quit()
    }

    C.Action {
        id: closeAction

        text: qsTr("Close")
        shortcut: StandardKey.Close
        onTriggered: window.close()
        enabled: Qt.platform.os != "android" && Qt.platform.os != "ios"
    }

    C.Action {
        id: findAction

        text: qsTr("&Find")
        shortcut: StandardKey.Find
        onTriggered: searchToolButtonAction.trigger()
    }

    C.Action {
        id: undoAction

        text: qsTr("Undo")
        shortcut: StandardKey.Undo
        enabled: {
            let currentItem = stackView.currentItem;
            if (currentItem && typeof(currentItem.undo) === "function") {
                return true;
            }
            return false;
        }
        onTriggered: {
            let currentItem = stackView.currentItem;
            currentItem.undo();
        }
    }

    Shortcut {
        id: goBackShortcut
        sequences: [
            StandardKey.Back,
            "Esc",
            "Back"
        ]
        enabled: Qt.platform.os !== "android" && Qt.platform.os !== "ios"
        onActivated: {
            if (stackView.canGoBack) {
                stackView.goBack();
            }
        }
    }

    C.Action {
        id: openLastCreatedItemAction

        shortcut: StandardKey.Open
        text: qsTr("Open Last &Created Item")
        enabled: !!window.itemCreatedNotification
        onTriggered: window.itemCreatedNotification.trigger()
    }

    C.Action {
        id: openLeftSideBarAction

        text: qsTr("Open &Left Side Bar")
        onTriggered: if (librariesSideBar.compact) {
                         dynamicLeftDrawer.visible = !dynamicLeftDrawer.visible;
                     }
    }

    Component.onCompleted: {
        librariesSideBar.lastLibrary = OTL.Application.loadValue("lastLibrary", "");
        librariesSideBar.lastTag = OTL.Application.loadValue("lastTag", "");
        librariesSideBar.lastSpecialView = OTL.Application.loadValue("specialView", "");

        d.completed = true;

        defaultFontSize = font.pointSize;
        font.pointSize = Qt.binding(function() {
            return AppSettings.useCustomFontSize ? AppSettings.customFontSize : defaultFontSize;
        });

        d.createQuickNoteEditor();
    }

    onClosing: {
        if (Qt.platform.os === "android") {
            close.accepted = false;
            if (stackView.canGoBack) {
                stackView.goBack();
                return;
            } else {
                // Actually, we should be able to just keep "close.accepted" on "true" and let
                // Qt close the app. BUT....
                //close.accepted = true;
                // This is not working. See https://gitlab.com/rpdev/opentodolist/-/issues/371
                // for more information. For some reason this causes the app to hang during shut
                // down. So we go the hard way and on the Java side ask the Activity to
                // stop the GUI. This seems to work reliably.
                OTL.Application.finishActivity();
            }
        } else {
            close.accepted = true;
        }
    }

    LibrariesSideBar {
        id: librariesSideBar

        function viewSelectedLibrary() {
            window.viewLibrary(currentLibrary, currentTag, specialView);
        }

        helpVisible: helpPage !== null
        anchors.fill: parent
        compact: window.width < 600
        onCurrentLibraryChanged: changeLibraryTimer.restart()
        onCurrentTagChanged: changeLibraryTimer.restart()
        onSpecialViewChanged: changeLibraryTimer.restart()
        onNewLibrary: {
            stackView.clear();
            stackView.push(newLibraryPage);
        }
        onAboutPageRequested: {
            if (helpPage) {
                stackView.pop(helpPage);
            } else {
                helpPage = stackView.push(
                            aboutPage, {
                                stack: stackView,
                                onClosed: function() {
                                    helpPage = null;
                                    librariesSideBar.helpVisible = false;
                                }
                            });
            }
        }
        onSettingsPageRequested: {
            if (settingsPage) {
                stackView.pop(settingsPage);
            } else {
                settingsPage = stackView.push(
                            settingsPageComponent, {
                                stack: stackView,
                                onClosed: function() {
                                    settingsPage = null;
                                    librariesSideBar.settingsVisible = false;
                                }
                            });
            }
        }
        onAccountsPageRequested: {
            if (accountsPage) {
                stackView.pop(accountsPage);
            } else {
                accountsPage = stackView.push(
                            accountsPageComponent, {
                                stack: stackView,
                                onClosed: function() {
                                    accountsPage = null;
                                    librariesSideBar.accountsVisible = false;
                                }
                            });
            }
        }

        parent: compact ? dynamicLeftDrawer.contentItem : staticLeftSideBar
        onClose: dynamicLeftDrawer.close()

        Timer {
            id: changeLibraryTimer
            interval: 100
            repeat: false
            onTriggered: librariesSideBar.viewSelectedLibrary()
        }
    }

    C.Pane {
        anchors.fill: stackView
        visible: librariesSideBar.numberOfLibraries === 0

        BackgroundLabel {
            text: Markdown.stylesheet +
                  qsTr("Start by <a href='#newLibrary'>creating a new " +
                       "library</a>. Libraries are used to store " +
                       "different kinds of items like notes, todo lists " +
                       "and images.")
            onLinkActivated: if (link === "#newLibrary") {
                                 newLibraryAction.trigger();
                             }
        }
    }

    C.StackView {
        id: stackView

        property bool hasSync: !!currentItem &&
                               (typeof(currentItem.syncRunning) === "boolean")
        property bool syncRunning: !!currentItem && !!currentItem.syncRunning
        property int syncProgress: {
            if (currentItem) {
                if (currentItem.syncProgress !== undefined) {
                    return currentItem.syncProgress;
                }
            }
            return -1;
        }

        property bool hasPageMenu: !!currentItem && !!currentItem.pageMenu
        property bool canGoBack: currentItem !== null &&
                                 (depth > 1 ||
                                  typeof(currentItem.goBack) === "function")

        property bool hasItem: currentItem &&
                               currentItem.item !== undefined
        property bool hasColor: hasItem && currentItem.item.color !== undefined
        property bool isCheckable: hasItem && currentItem.item.done !== undefined
        property bool isChecked: isCheckable ? currentItem.item.done : false

        function goBack(page) {
            if (page !== undefined && page !== null) {
                stackView.pop(page);
            } else if (typeof(currentItem.goBack) === "function") {
                currentItem.goBack();
            } else {
                stackView.pop();
            }
        }

        anchors {
            left: staticLeftSideBar.right
            top: parent.top
            bottom: parent.bottom
            right: parent.right
        }

        clip: true
        visible: depth > 0
        onCurrentItemChanged: applicationToolBar.closeMenu()
    }

    C.Pane {
        id: staticLeftSideBar
        width: librariesSideBar.compact ? 0 : Math.min(300, window.width / 3)
        height: parent.height
        Material.elevation: 6
        visible: width > 0
    }

    C.Drawer {
        id: dynamicLeftDrawer
        edge: Qt.LeftEdge
        width: Math.max(300, window.width / 3)
        height: window.height - window.header.height
        y: window.header.height
    }

    Connections {
        target: stackView.currentItem
        ignoreUnknownSignals: true

        function onClosePage() {
            stackView.goBack();
        }

        function onReturnToPage(page) {
            stackView.goBack(page);
        }

        function onOpenPage(component, properties) {
            stackView.push(component, properties);
        }

        function onClearAndOpenPage(component, properties) {
            stackView.clear();
            stackView.push(component, properties);
        }
    }

    UpdateNotificationBar {}

    Component {
        id: libraryPage
        LibraryPage {
            onItemClicked: {
                stackView.push(Qt.resolvedUrl("../Pages/" +
                                              item.itemType + "Page.qml"),
                               { item: OTL.Application.cloneItem(item), library: library, stack: stackView });
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
        id: settingsPageComponent

        SettingsPage {}
    }

    Component {
        id: accountsPageComponent

        AccountsPage {}
    }

    Component {
        id: problemsPage

        ProblemsPage {}
    }

    Component {
        id: newLibraryPage

        NewLibraryPage {
            onLibraryCreated: {
                if (library) {
                    librariesSideBar.currentLibrary = library;
                    librariesSideBar.currentTag = "";
                }
            }
        }
    }

    Connections {
        target: !!application ? application : null
        function onInstanceStarted() {
            console.warn("Instance started");
            window.show();
            window.requestActivate();
            window.raise();
        }
    }

    Connections {
        target: OTL.Application

        function onShowWindowRequested() {
            d.showMainWindow();
        }

        function onHideWindowRequested() {
            d.hideMainWindow();
        }

        function onShowQuickNotesEditorRequested() {
            d.showQuickNotesEditor();
        }

        function onSystemTrayIconClicked() {
            if (AppSettings.showQuickNotesEditorOnSystemTrayClick) {
                d.showQuickNotesEditor();
            } else {
                d.showMainWindow();
            }
        }

        function onApplicationActivated() {
            switch (Qt.platform.os) {
            case "android":
            case "ios":
                d.showMainWindow();
                break;
            default:
                break;
            }
        }
    }

    Connections {
        target: d.quickNoteWindow

        function onOpenMainWindow() {
            window.show();
            window.requestActivate();
            window.raise();
        }
    }

    Settings {
        category: "MainWindow"

        property alias width: window.width
        property alias height: window.height
    }

}

