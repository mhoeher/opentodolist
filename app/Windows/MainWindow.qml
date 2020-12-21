import QtQuick 2.9
import QtQuick.Window 2.3
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12
import Qt.labs.qmlmodels 1.0
import Qt.labs.settings 1.0

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
                id: pageMenuToolButton

                property var pageActions: {
                    if (stackView.currentItem && stackView.currentItem.pageActions) {
                        return stackView.currentItem.pageActions;
                    } else {
                        return [];
                    }
                }
                property int numPageActions: pageActions.length

                symbol: Icons.faEllipsisV
                visible: d.numVisibleDynamicPageMenuItems > 0 ||
                         numPageActions > 0
                Layout.alignment: Qt.AlignVCenter
                property var pageMenuEntries: {
                    let result = [];
                    let dynamicPageActions = d.visibleDynamicPageMenuItems;
                    for (var i = 0; i < dynamicPageActions.length; ++i) {
                        let item = dynamicPageActions[i];
                        result.push({"type": "item", "item": item});
                    }
                    if (dynamicPageActions.length > 0 && pageActions.length > 0) {
                        result.push({"type": "separator"})
                    }
                    for (i = 0; i < pageActions.length; ++i) {
                        let item = pageActions[i];
                        result.push({"type": "item", "item": item});
                    }
                    return result;
                }

                onPageMenuEntriesChanged: {
                    pageMenuEntriesModel.clear();
                    for (var i = 0; i < pageMenuEntries.length; ++i) {
                        pageMenuEntriesModel.append(pageMenuEntries[i]);
                    }
                }

                ListModel {
                    id: pageMenuEntriesModel
                    dynamicRoles: true
                }

                menu: Menu {
                    y: headerToolBar.height
                    modal: true
                    Repeater {
                        model: pageMenuEntriesModel
                        delegate: DelegateChooser {
                            role: "type"

                            DelegateChoice {
                                roleValue: "separator"
                                MenuSeparator {}
                            }

                            DelegateChoice {
                                roleValue: "item"
                                MenuItem {
                                    action: item
                                    visible: action.enabled
                                    height: visible ? implicitHeight : 0
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    Settings {
        property alias fontPixelSize: window.font.pixelSize

        category: "ApplicationWindow"
    }

    FontMetrics { id: fontMetrics; font: pageTitleLabel.font }

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
                    pageMenuToolButton.width;
            // Reserve some "minimum" space for the label:
            availableWidth -= Math.min(
                        fontMetrics.averageCharacterWidth * 30,
                        fontMetrics.boundingRect(pageTitleLabel.text).width);
            availableWidth -= 3 * toolBarLayout.spacing;

            // Cap to 0:
            return Math.max(availableWidth, 0);
        }

        property int numVisibleDynamicToolBarButtons: {
            var numButtons = widthForDynamicPageToolButtons /
                    (pageMenuToolButton.width + toolBarLayout.spacing);
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
                symbol: Icons.faCopy
                text: qsTr("Move")
                visible: {
                    return stackView.currentItem &&
                            typeof(stackView.currentItem["moveItem"]) ===
                            "function";
                }
                onTriggered: stackView.currentItem.moveItem()
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
            },

            ToolBarAction {
                symbol: Icons.faFire
                text: qsTr("Delete Completed Items")
                visible: stackView.currentItem && typeof(stackView.currentItem["deleteCompletedItems"]) === "function"
                onTriggered: stackView.currentItem.deleteCompletedItems()
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
        id: closeAction

        text: qsTr("Close")
        shortcut: StandardKey.Close
        onTriggered: window.close()
        enabled: Qt.platform.os != "android" && Qt.platform.os != "ios"
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
        enabled: Qt.platform.os !== "android" && Qt.platform.os !== "ios"
        onActivated: {
            if (stackView.canGoBack) {
                stackView.goBack();
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

        let maximized = OTL.Application.loadValue("maximized", false);
        console.log("Maximized: ", maximized, typeof(maximized));
        if (OTL.Application.loadValue("maximized", "false") === "true") {
            console.debug("Maximizing window on startup");
            window.visibility = Window.Maximized;
        }
        d.completed = true;
    }

    Component.onDestruction: {
        OTL.Application.saveValue("maximized", visibility === Window.Maximized);
        if (visibility === Window.Windowed) {
            OTL.Application.saveValue("width", width);
            OTL.Application.saveValue("height", height);
        }
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
        onCurrentItemChanged: pageMenuToolButton.menu.close()
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
            console.debug("Request to show main window")
            if (!window.visible) {
                window.show();
            }
            window.requestActivate();
            window.raise();
        }
    }
}

