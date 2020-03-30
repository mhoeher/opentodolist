import QtQuick 2.9
import QtQuick.Window 2.3
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12

import "../Components"
import "../Fonts"
import "../Widgets"
import "../Menues"
import "../Pages"
import "../Utils"

import OpenTodoList 1.0 as OTL

ApplicationWindow {
    id: window

    property ItemCreatedNotification itemCreatedNotification: null

    title: qsTr("OpenTodoList") + " - " + applicationVersion
    visible: true
    width: 640
    height: 480

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

    header: ToolBar {
        id: headerToolBar
        leftPadding: 10
        rightPadding: 10

        RowLayout {
            id: toolBarLayout

            width: parent.width
            height: parent.height

            ToolButton {
                id: sidebarControl
                symbol: Icons.faBars
                Layout.alignment: Qt.AlignVCenter
                visible: librariesSideBar.compact && stackView.depth <= 1
                checked: dynamicLeftDrawer.visible
                onClicked: dynamicLeftDrawer.visible = !dynamicLeftDrawer.visible
            }

            ToolButton {
                id: backToolButton

                symbol: Icons.faArrowLeft
                visible: stackView.canGoBack
                onClicked: stackView.goBack()
                Layout.alignment: Qt.AlignVCenter
            }

            ToolButton {
                id: problemsButton

                symbol: Icons.faExclamationTriangle
                visible: OTL.Application.problemManager.problems.length > 0
                onClicked: stackView.replace(problemsPage)
                Material.foreground: Colors.negativeColor
                Layout.alignment: Qt.AlignVCenter
            }

            BusyIndicator {
                id: busyIndicator

                visible: stackView.syncRunning
                implicitHeight: backToolButton.height
                implicitWidth: implicitHeight
                hoverEnabled: true

                ToolTip.visible: busyIndicator.hovered
                ToolTip.delay: 1000
                ToolTip.timeout: 5000
                ToolTip.text: qsTr("Synchronizing library...")
            }

            Label {
                id: pageTitleLabel
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignVCenter
                wrapMode: Text.NoWrap

                elide: Text.ElideRight

                Binding {
                    target: pageTitleLabel
                    property: "text"
                    value: stackView.currentItem ? stackView.currentItem.title : ""
                }
            }

            Repeater {
                model: d.visibleDynamicToolBarButtons

                delegate: ToolButton {
                    action: modelData
                    menu: action.menu
                    symbol: action.symbol
                    visible: action.visible
                    Layout.alignment: Qt.AlignVCenter
                }
            }

            ToolButton {
                id: dynamicPageActionsMenu
                visible: d.visibleDynamicPageMenuItems.length > 0
                symbol: Icons.faChevronDown
                Layout.alignment: Qt.AlignVCenter
                menu: Menu {
                    y: headerToolBar.height
                    Repeater {
                        model: d.visibleDynamicPageMenuItems
                        delegate: MenuItem {
                            action: modelData
                            visible: action.visible
                        }
                    }
                }
            }

            ToolButton {
                id: pageMenuToolButton

                symbol: Icons.faEllipsisV
                visible: stackView.hasPageMenu
                Layout.alignment: Qt.AlignVCenter
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

        // The list of all available page actions
        property list<ToolBarAction> dynamicPageActions

        // The list of all page actions which are visible
        property var visibleDynamicPageActions

        property int widthForDynamicPageToolButtons: {
            // Calculate the space available for dynamic tool buttons:
            var availableWidth = toolBarLayout.width;
            // We need some space for the "static" buttons:
            availableWidth -=
                    sidebarControl.width * (sidebarControl.visible ? 1 : 0) +
                    problemsButton.width * (problemsButton.visible ? 1 : 0) +
                    backToolButton.width * (backToolButton.visible ? 1 : 0) +
                    busyIndicator.implicitWidth * (busyIndicator.visible ? 1 : 0) +
                    dynamicPageActionsMenu.width +
                    pageMenuToolButton.width * (pageMenuToolButton.visible ? 1 : 0);
            // Reserve some "minimum" space for the label:
            availableWidth -= Math.min(
                    pageMenuToolButton.width * 4,
                        pageTitleLabel.contentWidth);

            // Cap to 0:
            return Math.max(availableWidth, 0);
        }

        property int numVisibleDynamicToolBarButtons: {
            var numButtons = widthForDynamicPageToolButtons /
                    pageMenuToolButton.width;
            numButtons = parseInt(numButtons);
            // Cap to maximum number of available actions:
            numButtons = Math.min(numButtons, visibleDynamicPageActions.length);
            return numButtons;
        }

        property int numVisibleDynamicPageMenuItems: {
            return visibleDynamicPageActions.length -
                    numVisibleDynamicToolBarButtons;
        }

        property var visibleDynamicToolBarButtons
        property var visibleDynamicPageMenuItems

        dynamicPageActions: [
            ToolBarAction {
                symbol: Icons.faPencilAlt
                text: qsTr("Rename")
                visible: {
                    return stackView.currentItem &&
                            typeof(stackView.currentItem["renameItem"]) ===
                            "function";
                }
                onTriggered: stackView.currentItem.renameItem()
            },

            ToolBarAction {
                symbol: Icons.faPaintBrush
                text: qsTr("Color")
                menu: ColorMenu {
                    anchors.centerIn: parent
                    item: stackView.hasColor ? stackView.currentItem.item : null
                    parent: window.contentItem
                }
                visible: menu.item !== null
            },

            ToolBarAction {
                symbol: Icons.faTag
                text: qsTr("Add Tag")
                visible: stackView.currentItem && (typeof(stackView.currentItem["addTag"]) === "function")
                onTriggered: stackView.currentItem.addTag()
            },

            ToolBarAction {
                symbol: Icons.faPaperclip
                text: qsTr("Attach File")
                visible: stackView.currentItem && (typeof(stackView.currentItem["attach"]) === "function")
                onTriggered: stackView.currentItem.attach()
            },

            ToolBarAction {
                id: searchToolButtonAction

                symbol: Icons.faSearch
                text: qsTr("Search")
                visible: stackView.currentItem && (typeof(stackView.currentItem["find"]) === "function")
                onTriggered: stackView.currentItem.find()
            },

            ToolBarAction {
                symbol: Icons.faSort
                text: qsTr("Sort")
                visible: stackView.currentItem &&
                         typeof(stackView.currentItem.sort) === "function"
                onTriggered: stackView.currentItem.sort()
            },

            ToolBarAction {
                symbol: Icons.faCalendarCheck
                text: qsTr("Set Due Date")
                visible: stackView.currentItem &&
                         typeof(stackView.currentItem.setDueDate) === "function"
                onTriggered: stackView.currentItem.setDueDate()
            },

            ToolBarAction {
                symbol: Icons.faTrashAlt
                text: qsTr("Delete")
                visible: stackView.currentItem && typeof(stackView.currentItem["deleteItem"]) === "function"
                onTriggered: stackView.currentItem.deleteItem()
            }
        ]

        visibleDynamicPageActions: {
            var result = [];
            for (var i = 0; i < dynamicPageActions.length; ++i) {
                var action = dynamicPageActions[i];
                if (action.visible) {
                    result.push(action);
                }
            }
            return result;
        }

        visibleDynamicToolBarButtons: {
            var result = [];
            for (var i = 0; i < numVisibleDynamicToolBarButtons; ++i) {
                var action = visibleDynamicPageActions[i];
                result.push(action);
            }
            return result;
        }

        visibleDynamicPageMenuItems: {
            var result = [];
            for (var i = 0; i < numVisibleDynamicPageMenuItems; ++i) {
                var action = visibleDynamicPageActions[
                        numVisibleDynamicToolBarButtons + i];
                result.push(action);
            }
            return result;
        }
    }

    Action {
        text: qsTr("Settings")
        shortcut: qsTr("Ctrl+,")
        onTriggered: librariesSideBar.showSettings()
    }

    Action {
        id: newLibraryAction

        text: qsTr("New &Library")
        shortcut: StandardKey.New
        onTriggered: {
            stackView.clear();
            stackView.push(newLibraryPage);
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
        onTriggered: searchToolButtonAction.clicked(null)
    }

    Shortcut {
        id: goBackShortcut
        sequences: [
            StandardKey.Back,
            "Esc",
            "Back"
        ]
        onActivated: {
            if (stackView.canGoBack) {
                stackView.goBack();
            } else {
                if (Qt.platform.os === "android") {
                    Qt.quit();
                }
            }
        }
    }

    Action {
        id: openLastCreatedItemAction

        shortcut: StandardKey.Open
        text: qsTr("Open Last &Created Item")
        enabled: !!window.itemCreatedNotification
        onTriggered: window.itemCreatedNotification.trigger()
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

        librariesSideBar.lastLibrary = OTL.Application.loadValue("lastLibrary", "");
        librariesSideBar.lastTag = OTL.Application.loadValue("lastTag", "");
        librariesSideBar.lastSpecialView = OTL.Application.loadValue("specialView", "");

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
        d.completed = true;
    }

    onClosing: {
        if (Qt.platform.os === "android") {
            if (stackView.canGoBack) {
                stackView.goBack();
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

    Pane {
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

    StackView {
        id: stackView

        property bool hasSync: !!currentItem &&
                               (typeof(currentItem.syncRunning) === "boolean")
        property bool syncRunning: !!currentItem && !!currentItem.syncRunning
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
    }

    Pane {
        id: staticLeftSideBar
        width: librariesSideBar.compact ? 0 : Math.min(300, window.width / 3)
        height: parent.height
        Material.elevation: 6
        visible: width > 0
    }

    Drawer {
        id: dynamicLeftDrawer
        edge: Qt.LeftEdge
        width: Math.max(300, window.width / 3)
        height: window.height - window.header.height
        y: window.header.height
    }

    Connections {
        target: stackView.currentItem
        ignoreUnknownSignals: true
        onClosePage: stackView.goBack()
        onReturnToPage: stackView.goBack(page)
        onOpenPage: stackView.push(component, properties)
        onClearAndOpenPage: {
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

//            onCancelled: window.viewLibrary()
//            onLibraryAvailable: {
//                var lib = OTL.Application.addLibrary(synchronizer);
//                if (lib !== null) {
//                    librariesSideBar.currentLibrary = lib;
//                    librariesSideBar.currentTag = "";
//                } else {
//                    console.error("Failed to create library!");
//                    window.viewLibrary();
//                }
//            }
        }
    }

    Connections {
        target: !!application ? application : null
        onInstanceStarted: {
            console.warn("Instance started");
            window.show();
            window.raise();
        }
    }

}
