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
    onVisibleChanged: if (visible) {
                          OTL.Application.syncAllLibraries()
                      }

    function viewLibrary(lib, tag, special) {
        stackView.clear()
        if (lib) {
            switch (special) {
            case "schedule":
                stackView.push(scheduleViewPage, {
                                   "library": lib
                               })
                break
            default:
                stackView.push(libraryPage, {
                                   "library": lib,
                                   "tag": tag
                               })
                break
            }
            if (d.completed) {
                OTL.Application.saveValue("lastLibrary", lib.uid.toString())
                OTL.Application.saveValue("lastTag", tag)
                OTL.Application.saveValue("specialView", special)
            }
        } else if (lib === null) {
            // Special case: If lib is null, check if a global view has been requested
            switch (special) {
            case "schedule":
                stackView.push(scheduleViewPage, {})
                if (d.completed) {
                    OTL.Application.saveValue("lastLibrary", "")
                    OTL.Application.saveValue("lastTag", "")
                    OTL.Application.saveValue("specialView", special)
                }
                break
            default:
                break
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

        backToolButton.visible: stackView.depth > 1
        backToolButton.onClicked: stackView.goBack()

        problemsButton.visible: OTL.Application.problemManager.problems.length > 0
        problemsButton.onClicked: stackView.replace(problemsPage)

        busyIndicator.visible: stackView.syncRunning

        pageActions: {
            if (stackView.currentItem && stackView.currentItem.pageActions) {
                return stackView.currentItem.pageActions
            } else {
                return []
            }
        }
        dynamicPageActions: applicationToolBarActions.dynamicPageActions

        Binding {
            target: applicationToolBar
            property: "title"
            value: stackView.currentItem ? stackView.currentItem.title : ""
        }
    }

    QtObject {
        id: d

        property bool completed: false

        property QuickNoteWindow quickNoteWindow: null

        function createQuickNoteEditor() {
            if (!AppSettings.supportsQuickEditor) {
                return
            }

            let componentUrl = Qt.resolvedUrl("./QuickNoteWindow.qml")
            let component = Qt.createComponent(componentUrl)
            quickNoteWindow = component.createObject()
        }

        function showMainWindow() {
            if (!window.visible) {
                window.show()
            }
            window.requestActivate()
            window.raise()
        }

        function hideMainWindow() {
            window.hide()
        }

        function showQuickNotesEditor() {
            quickNoteWindow.show()
            quickNoteWindow.requestActivate()
            quickNoteWindow.raise()
        }
    }

    ApplicationShortcuts {
        id: applicationShortcuts

        settingsShortcut.onActivated: librariesSideBar.showSettings()
        quitShortcut.onActivated: Qt.quit()
        closeShortcut.onActivated: window.close()
        closeShortcut.enabled: Qt.platform.os != "android"
                               && Qt.platform.os != "ios"
        findShortcut.enabled: {
            let currentItem = stackView.currentItem
            if (currentItem && typeof (currentItem.find) === "function") {
                return true
            }
            return false
        }
        undoShortcut.enabled: {
            let currentItem = stackView.currentItem
            if (currentItem && typeof (currentItem.undo) === "function") {
                return true
            }
            return false
        }
        undoShortcut.onActivated: {
            let currentItem = stackView.currentItem
            currentItem.undo()
        }
        goBackShortcut.onActivated: {
            if (stackView.canGoBack) {
                stackView.goBack()
            } else {
                // We are at the top of the stack. If the window is in fullscreen mode, go to
                // "default" mode (which should usually be "windowed" on most systems).
                if (window.visibility === Window.FullScreen) {
                    window.visibility = Window.AutomaticVisibility
                }
            }
        }
        openShortcut.enabled: !!window.itemCreatedNotification
        openShortcut.onActivated: window.itemCreatedNotification.trigger()
    }

    ApplicationToolBarActions {
        id: applicationToolBarActions
        shortcuts: applicationShortcuts
    }

    Component.onCompleted: {
        librariesSideBar.lastLibrary = OTL.Application.loadValue("lastLibrary",
                                                                 "")
        librariesSideBar.lastTag = "" + OTL.Application.loadValue("lastTag", "")
        librariesSideBar.lastSpecialView = "" + OTL.Application.loadValue(
                    "specialView", "")

        d.completed = true

        defaultFontSize = font.pointSize
        font.pointSize = Qt.binding(function () {
            return AppSettings.useCustomFontSize ? AppSettings.customFontSize : defaultFontSize
        })

        d.createQuickNoteEditor()
    }

    onClosing: {
        switch (Qt.platform.os) {
        case "android":
            if (stackView.canGoBack) {
                close.accepted = false
                stackView.goBack()
                return
            } else {
                // Actually, we should be able to just keep "close.accepted" on "true" and let
                // Qt close the app. BUT....
                //close.accepted = true;
                // This is not working. See https://gitlab.com/rpdev/opentodolist/-/issues/371
                // for more information. For some reason this causes the app to hang during shut
                // down. So we go the hard way and on the Java side ask the Activity to
                // stop the GUI. This seems to work reliably.
                OTL.Application.finishActivity()
            }
            break
        default:
            close.accepted = true
            break
        }
    }

    LibrariesSideBar {
        id: librariesSideBar

        helpVisible: helpPage != null
        anchors.fill: parent
        compact: window.width < 600
        onShowLibrary: window.viewLibrary(library, tag, specialView)
        onNewLibrary: {
            stackView.clear()
            stackView.push(newLibraryPage)
        }
        onAboutPageRequested: {
            if (helpPage) {
                stackView.pop(helpPage)
            } else {
                helpPage = stackView.push(aboutPage, {
                                              "stack": stackView,
                                              "onClosed": function () {
                                                  helpPage = null
                                                  librariesSideBar.helpVisible = false
                                              }
                                          })
            }
        }
        onSettingsPageRequested: {
            if (settingsPage) {
                stackView.pop(settingsPage)
            } else {
                settingsPage = stackView.push(settingsPageComponent, {
                                                  "stack": stackView,
                                                  "onClosed": function () {
                                                      settingsPage = null
                                                      librariesSideBar.settingsVisible = false
                                                  }
                                              })
            }
        }
        onAccountsPageRequested: {
            if (accountsPage) {
                stackView.pop(accountsPage)
            } else {
                accountsPage = stackView.push(accountsPageComponent, {
                                                  "stack": stackView,
                                                  "onClosed": function () {
                                                      accountsPage = null
                                                      librariesSideBar.accountsVisible = false
                                                  }
                                              })
            }
        }

        parent: compact ? dynamicLeftDrawer.contentItem : staticLeftSideBar
        onClose: dynamicLeftDrawer.close()
    }

    C.Pane {
        anchors.fill: stackView
        visible: librariesSideBar.numberOfLibraries === 0

        BackgroundLabel {
            text: Markdown.stylesheet + qsTr(
                      "Start by <a href='#newLibrary'>creating a new "
                      + "library</a>. Libraries are used to store "
                      + "different kinds of items like notes, todo lists " + "and images.")
            onLinkActivated: if (link === "#newLibrary") {
                                 stackView.clear()
                                 stackView.push(newLibraryPage)
                             }
        }
    }

    C.StackView {
        id: stackView

        property bool hasSync: !!currentItem
                               && (typeof (currentItem.syncRunning) === "boolean")
        property bool syncRunning: !!currentItem && !!currentItem.syncRunning
        property int syncProgress: {
            if (currentItem) {
                if (currentItem.syncProgress !== undefined) {
                    return currentItem.syncProgress
                }
            }
            return -1
        }

        property bool hasPageMenu: !!currentItem && !!currentItem.pageMenu
        property bool canGoBack: currentItem !== null
                                 && (depth > 1
                                     || typeof (currentItem.goBack) === "function")

        property bool hasItem: currentItem && currentItem.item !== undefined
        property bool hasColor: hasItem && currentItem.item.color !== undefined
        property bool isCheckable: hasItem
                                   && currentItem.item.done !== undefined
        property bool isChecked: isCheckable ? currentItem.item.done : false

        function goBack(page) {
            if (page !== undefined && page !== null) {
                stackView.pop(page)
            } else if (typeof (currentItem.goBack) === "function") {
                currentItem.goBack()
            } else {
                stackView.pop()
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

    DropArea {
        anchors.fill: stackView
        keys: ["text/uri-list"]

        onEntered: drag => {
                       if (drag.hasUrls) {
                           if (typeof (stackView.currentItem.attachFiles) == "function") {
                               drag.accept(Qt.LinkAction)
                               dropIndicator.visible = true
                           }
                       }
                   }
        onDropped: drop => {
                       stackView.currentItem.attachFiles(drop.urls)
                   }
        onExited: {
            dropIndicator.visible = false
        }
        onContainsDragChanged: {
            if (!containsDrag) {
                dropIndicator.visible = false
            }
        }
    }

    Rectangle {
        id: dropIndicator
        anchors.fill: stackView
        color: "white"
        opacity: 0.5
        visible: false
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
            stackView.goBack()
        }

        function onReturnToPage(page) {
            stackView.goBack(page)
        }

        function onOpenPage(component, properties) {
            stackView.push(component, properties)
        }

        function onClearAndOpenPage(component, properties) {
            stackView.clear()
            stackView.push(component, properties)
        }
    }

    UpdateNotificationBar {}

    Component {
        id: libraryPage
        LibraryPage {
            onItemClicked: {
                stackView.push(Qt.resolvedUrl(
                                   "../Pages/" + item.itemType + "Page.qml"), {
                                   "item": OTL.Application.cloneItem(item),
                                   "library": library,
                                   "stack": stackView
                               })
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
                    librariesSideBar.currentLibrary = library
                    librariesSideBar.currentTag = ""
                    viewLibrary(library)
                }
            }
        }
    }

    Connections {
        target: !!application ? application : null
        function onInstanceStarted() {
            console.warn("Instance started")
            window.show()
            window.requestActivate()
            window.raise()
        }
    }

    Connections {
        target: OTL.Application

        function onShowWindowRequested() {
            d.showMainWindow()
        }

        function onHideWindowRequested() {
            d.hideMainWindow()
        }

        function onShowQuickNotesEditorRequested() {
            d.showQuickNotesEditor()
        }

        function onSystemTrayIconClicked() {
            if (AppSettings.showQuickNotesEditorOnSystemTrayClick) {
                d.showQuickNotesEditor()
            } else {
                d.showMainWindow()
            }
        }

        function onApplicationActivated() {
            switch (Qt.platform.os) {
            case "android":
            case "ios":
            case "osx":
                // https://gitlab.com/rpdev/opentodolist/-/issues/518
                d.showMainWindow()
                break
            default:
                break
            }
        }
    }

    Connections {
        target: d.quickNoteWindow

        function onOpenMainWindow() {
            window.show()
            window.requestActivate()
            window.raise()
        }
    }

    Settings {
        category: "MainWindow"

        property alias width: window.width
        property alias height: window.height
    }
}
