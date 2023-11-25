import QtQuick
import "../Controls" as C
import "../Fonts" as Fonts
import "../Pages" as Pages
import "../"

import OpenTodoList as OTL

Item {
    id: shortcuts

    property bool isSecondaryWindow: false

    property C.StackView stackView
    property C.ApplicationWindow window

    property C.Action aboutApp: C.Action {
        id: aboutApp
        text: qsTr("About")
        shortcut: StandardKey.HelpContents
        symbol: Fonts.Icons.mdiInfo
        enabled: !isSecondaryWindow
        onTriggered: stackView.clearAndOpenPage(Qt.resolvedUrl(
                                                    "../Pages/AboutPage.qml"))
    }

    property C.Action aboutQt: C.Action {
        text: qsTr("About Qt")
        symbol: Fonts.Icons.mdiInfo
        enabled: !isSecondaryWindow && Qt.platform.os !== "ios"
                 && Qt.platform.os !== "android"
        onTriggered: OTL.Application.aboutQt()
    }

    property C.Action accounts: C.Action {
        text: qsTr("Accounts")
        enabled: !isSecondaryWindow
        symbol: Fonts.Icons.mdiAccountCircle
        onTriggered: stackView.clearAndOpenPage(
                         Qt.resolvedUrl("../Pages/AccountsPage.qml"))
    }

    property C.Action addTag: C.Action {
        text: qsTr("Add Tag")
        shortcut: "Ctrl+Shift+T"
        symbol: Fonts.Icons.mdiLabel
        enabled: typeof (stackView?.currentItem?.addTag) === "function"
        onTriggered: stackView.currentItem.addTag()
    }

    property C.Action attachFile: C.Action {
        text: qsTr("Attach File")
        shortcut: "Ctrl+Shift+A"
        symbol: Fonts.Icons.mdiAttachment
        enabled: typeof (stackView?.currentItem?.attach) === "function"
        onTriggered: stackView.currentItem.attach()
    }

    property C.Action close: C.Action {
        text: qsTr("Close")
        shortcut: StandardKey.Close
        enabled: Qt.platform.os !== "android" && Qt.platform.os !== "ios"
        onTriggered: window.close()
    }

    property C.Action copy: C.Action {
        text: qsTr("Copy")
        shortcut: "Ctrl+Shift+C"
        symbol: Fonts.Icons.mdiContentCopy
        enabled: typeof (stackView?.currentItem?.copyItem) === "function"
        onTriggered: stackView.currentItem.copyItem()
    }

    property C.Action deleteItem: C.Action {
        text: qsTr("Delete")
        shortcut: StandardKey.Delete
        symbol: Fonts.Icons.mdiDelete
        enabled: typeof (stackView?.currentItem?.deleteItem) === "function"
        onTriggered: stackView.currentItem.deleteItem()
    }

    property C.Action deleteCompletedItems: C.Action {
        text: qsTr("Delete Completed Items")
        shortcut: "Ctrl+Shift+Del"
        symbol: Fonts.Icons.mdiRemoveDone
        enabled: typeof (stackView?.currentItem?.deleteCompletedItems) === "function"
        onTriggered: stackView.currentItem.deleteCompletedItems()
    }

    property C.Action dueDate: C.Action {
        text: qsTr("Due Date")
        shortcut: "Ctrl+Shift+D"
        symbol: Fonts.Icons.mdiCalendarToday
        enabled: typeof (stackView?.currentItem?.setDueDate) === "function"
        onTriggered: stackView.currentItem.setDueDate()
    }

    property C.Action find: C.Action {
        text: qsTr("Find")
        shortcut: StandardKey.Find
        enabled: typeof (stackView?.currentItem?.find) === "function"
        onTriggered: stackView?.currentItem?.find()
    }

    property C.Action goBack: C.Action {
        text: qsTr("Back")
        shortcut: StandardKey.Back
        enabled: Qt.platform.os !== "android" && Qt.platform.os !== "ios"
        onTriggered: {
            if (stackView?.canGoBack) {
                stackView.goBack()
            } else {
                // We are at the top of the stack. If the window is in fullscreen mode, go to
                // "default" mode (which should usually be "windowed" on most systems).
                if (window.visibility === Window.FullScreen) {
                    window.visibility = Window.AutomaticVisibility
                }
            }
        }
    }

    property C.Action leftSidebar: C.Action {
        text: qsTr("Left Sidebar")
        shortcut: "Ctrl+0"
        symbol: Fonts.Icons.mdiMenu
    }

    property C.Action markAllItemsAsDone: C.Action {
        text: qsTr("Mark all items as done")
        enabled: !!stackView?.topmostTodo || !!stackView?.topmostTodoList
        onTriggered: {
            if (stackView.topmostTodo) {
                OTL.Application.markAllItemsAsDone(stackView.topmostTodo)
            } else if (stackView.topmostTodoList) {
                OTL.Application.markAllItemsAsDone(stackView.topmostTodoList)
            }
        }
    }

    property C.Action markAllItemsAsUndone: C.Action {
        text: qsTr("Mark all items as undone")
        enabled: !!stackView?.topmostTodo || !!stackView?.topmostTodoList
        onTriggered: {
            if (stackView.topmostTodo) {
                OTL.Application.markAllItemsAsUndone(stackView.topmostTodo)
            } else if (stackView.topmostTodoList) {
                OTL.Application.markAllItemsAsUndone(stackView.topmostTodoList)
            }
        }
    }

    property C.Action move: C.Action {
        text: qsTr("Move")
        shortcut: "Ctrl+Shift+M"
        symbol: Fonts.Icons.mdiCut
        enabled: typeof (stackView?.currentItem?.moveItem) === "function"
        onTriggered: stackView.currentItem.moveItem()
    }

    property C.Action newLibrary: C.Action {
        text: qsTr("New Library")
        shortcut: "Ctrl+Shift+N"
        enabled: !isSecondaryWindow
        symbol: Fonts.Icons.mdiAdd
        onTriggered: stackView.clearAndOpenPage(newLibraryPage)
    }

    property C.Action open: C.Action {
        text: qsTr("Open Created Item")
        shortcut: StandardKey.Open
        enabled: !!window?.itemCreatedNotification
        onTriggered: window.itemCreatedNotification.trigger()
    }

    property C.Action openInNewWindow: C.Action {
        text: qsTr("Open In New Window")
        shortcut: StandardKey.AddTab
        enabled: !isSecondaryWindow && Application.supportsMultipleWindows
                 && typeof (stackView?.currentItem?.openInNewWindow) === "function"
        onTriggered: stackView.currentItem.openInNewWindow()
    }

    property C.Action openLibraryFolder: C.Action {
        text: qsTr("Open Library Folder")
        enabled: !!stackView.topmostLibrary
        onTriggered: shareUtils.openFolder(stackView.topmostLibrary.directory)
    }

    property C.Action quit: C.Action {
        text: qsTr("Quit")
        shortcut: [StandardKey.Quit]
        onTriggered: Qt.quit()
    }

    property C.Action rename: C.Action {
        text: qsTr("Rename")
        shortcut: "Ctrl+Shift+R"
        symbol: Fonts.Icons.mdiEdit
        enabled: typeof (stackView?.currentItem?.renameItem) === "function"
        onTriggered: stackView.currentItem.renameItem()
    }

    property C.Action settings: C.Action {
        text: qsTr("Preferences")
        enabled: !isSecondaryWindow
        shortcut: "Ctrl+,"
        symbol: Fonts.Icons.mdiSettings
        onTriggered: {
            if (!stackView) {
                return
            }

            for (var i = 0; i < stackView.depth; ++i) {
                let page = stackView.get(i)
                if (page instanceof Pages.SettingsPage) {
                    stackView.pop(page)
                    return
                }
            }
            stackView.push(Qt.resolvedUrl("../Pages/SettingsPage.qml"))
        }
    }

    property C.Action scrollToTop: C.Action {
        text: qsTr("Scroll to Top")
        shortcut: "Home"
        enabled: typeof (stackView?.currentItem?.scrollToTop) === "function"
        onTriggered: stackView?.currentItem?.scrollToTop()
    }

    property C.Action scrollToBottom: C.Action {
        text: qsTr("Scroll to Bottom")
        shortcut: "End"
        enabled: typeof (stackView?.currentItem?.scrollToBottom) === "function"
        onTriggered: stackView?.currentItem?.scrollToBottom()
    }

    property C.Action setColor: C.Action {
        text: qsTr("Set Color")
        symbol: Fonts.Icons.mdiPalette
        enabled: !!stackView?.topmostPageWithSelectColorFunction
        onTriggered: stackView.topmostPageWithSelectColorFunction.selectColor()
    }

    property C.Action setProgress: C.Action {
        text: qsTr("Set Progress")
        symbol: Fonts.Icons.mdiPublishedWithChanges
        enabled: typeof (stackView?.currentItem?.setProgress) === "function"
        onTriggered: stackView?.currentItem?.setProgress()
    }

    property C.Action sort: C.Action {
        text: qsTr("Sort")
        shortcut: "Ctrl+Shift+S"
        symbol: Fonts.Icons.mdiSort
        enabled: typeof (stackView?.currentItem?.sort) === "function"
        onTriggered: stackView?.currentItem?.sort()
    }

    property C.Action sync: C.Action {
        text: qsTr("Sync Now")
        shortcut: StandardKey.Refresh
        symbol: Fonts.Icons.mdiSync
        onTriggered: {
            if (stackView?.topmostLibrary) {
                OTL.Application.syncLibrary(stackView.topmostLibrary)
            } else {
                OTL.Application.syncAllLibraries()
            }
        }
    }

    property C.Action syncLog: C.Action {
        text: qsTr("Sync Log")
        enabled: !!stackView?.topmostLibrary
                 && !(stackView?.currentItem instanceof Pages.LogViewPage)
        onTriggered: stackView.push(Qt.resolvedUrl("../Pages/LogViewPage.qml"),
                                    {
                                        "log": stackView.topmostLibrary.syncLog(
                                                   )
                                    })
    }

    property C.Action translateTheApp: C.Action {
        text: qsTr("Translate The App...")
        enabled: !isSecondaryWindow
        symbol: Fonts.Icons.mdiTranslate
        onTriggered: shareUtils.openLink(
                         "https://poeditor.com/join/project/ztvOymGNxn")
    }

    signal newLibraryCreated(var library)

    Shortcut {
        sequences: ["Esc", "Back"]
        onActivated: shortcuts.goBack.triggered()
    }

    OTL.ShareUtils {
        id: shareUtils
    }

    Component {
        id: newLibraryPage

        Pages.NewLibraryPage {
            onLibraryCreated: library => {
                                  if (library) {
                                      newLibraryCreated(library)
                                  }
                              }
        }
    }
}
