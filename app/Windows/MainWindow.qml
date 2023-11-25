import QtQuick 2.10
import QtQuick.Window 2.3
import QtQuick.Layouts 1.1
import QtQuick.Controls.Material 2.12
import Qt.labs.qmlmodels 1.0
import QtCore

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

    function viewLibrary(lib, attributes) {
        stackView.clear()
        if (!attributes) {
            attributes = {}
        }
        attributes.library = lib

        stackView.push(Qt.resolvedUrl("../Pages/LibraryPage.qml"), attributes)
    }

    function viewItem(library, item) {
        stackView.clear()
        stackView.push(Qt.resolvedUrl(
                           "../Pages/" + item.itemType + "Page.qml"), {
                           "item": OTL.Application.cloneItem(item),
                           "library": library
                       })
    }

    function viewSchedule(lib) {
        stackView.clear()
        stackView.push(Qt.resolvedUrl("../Pages/ScheduleViewPage.qml"), {
                           "library": lib
                       })
    }

    header: ApplicationToolBar {
        id: applicationToolBar

        stackView: stackView
        appMenu: appMenu
        appShortcuts: applicationShortcuts

        sidebarControl.visible: applicationShortcuts.leftSidebar.enabled
                                && stackView.depth <= 1

        backToolButton.visible: stackView?.depth > 1
        backToolButton.onClicked: stackView.goBack()

        problemsButton.visible: OTL.Application.problemManager.problems.length > 0
        problemsButton.onClicked: stackView.replace(problemsPage)

        Binding {
            target: applicationToolBar
            property: "title"
            value: stackView?.currentItem?.title ?? ""
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

    ApplicationMenu {
        id: appMenu
        shortcuts: applicationShortcuts
    }

    ApplicationShortcuts {
        id: applicationShortcuts

        stackView: stackView
        window: window

        leftSidebar.enabled: librariesSideBar?.compact
        leftSidebar.checked: dynamicLeftDrawer?.visible ?? false
        leftSidebar.onTriggered: dynamicLeftDrawer.visible = !dynamicLeftDrawer.visible

        onNewLibraryCreated: library => viewLibrary(library)
    }

    Component.onCompleted: {
        d.completed = true

        defaultFontSize = font.pointSize
        font.pointSize = Qt.binding(function () {
            return AppSettings.useCustomFontSize ? AppSettings.customFontSize : defaultFontSize
        })

        d.createQuickNoteEditor()
    }

    onClosing: close => {
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

        anchors.fill: parent
        appShortcuts: applicationShortcuts
        compact: window.width < 600
        stack: stackView
        onShowLibrary: (library, attributes) => window.viewLibrary(library,
                                                                   attributes)
        onShowItem: (library, item) => window.viewItem(library, item)
        onShowSchedule: library => window.viewSchedule(library)

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
                                 applicationShortcuts.newLibrary.triggered()
                             }
        }
    }

    C.StackView {
        id: stackView

        property bool hasPageMenu: !!currentItem && !!currentItem.pageMenu

        property bool hasItem: currentItem && currentItem.item !== undefined

        anchors {
            left: staticLeftSideBar.right
            top: parent.top
            bottom: parent.bottom
            right: parent.right
        }

        clip: true
        visible: depth > 0
        stackId: "mainWindow"
        startPageComponent: Qt.resolvedUrl("../Pages/StartPage.qml")
        onCurrentItemChanged: {
            librariesSideBar.close()
        }
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

    UpdateNotificationBar {}

    Component {
        id: problemsPage

        ProblemsPage {}
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
