import QtQuick

import "./Windows" as Windows
import "./Controls" as C

import OpenTodoList as OTL

QtObject {
    id: rootObject

    property Windows.MainWindow mainWindow: Windows.MainWindow {
        visible: true
    }

    function newStackViewWindow(page, props) {
        var window = stackViewWindow.createObject(rootObject)
        window.openInitialPage(page, props)
        window.show()
        window.raise()
        secondaryWindows.push(window)
    }

    property var stackViewWindow: Component {
        Windows.StackViewWindow {
            id: stackViewWin

            onVisibilityChanged: {
                if (stackViewWin.visibility === Window.Hidden) {
                    let windows = []
                    secondaryWindows.forEach(window => {
                                                 if (window !== stackViewWin) {
                                                     windows.push(window)
                                                 }
                                             })
                    secondaryWindows = windows
                }
            }
        }
    }

    property var secondaryWindows: []


    /**
     * @brief Get the currently active window.
     *
     * This returns the window that currently has focus or - in case none is focused -
     * the main window.
     */
    function getActiveWindow() {
        if (rootObject.mainWindow.active) {
            return rootObject.mainWindow
        }
        for (var i = 0; i < rootObject.secondaryWindows.length; ++i) {
            let window = rootObject.secondaryWindows[i]
            if (window.active) {
                return window
            }
        }
        return rootObject.mainWindow
    }


    /**
     * @brief Handle deep links.
     *
     * Here we handle opening of deep links into the application.
     */
    property var appConnections: Connections {
        target: OTL.Application

        function onOpenLinkToLibrary(url, uid) {
            rootObject.d.addTransaction({
                                            "uid": uid,
                                            "url": url,
                                            "transactionId": OTL.Application.loadLibrary(
                                                                 uid),
                                            "type": "library"
                                        })
        }

        function onOpenLinkToItem(url, uid) {
            rootObject.d.addTransaction({
                                            "uid": uid,
                                            "url": url,
                                            "transactionId": OTL.Application.loadItem(
                                                                 uid),
                                            "type": "item"
                                        })
        }

        function onLibraryLoaded(uid, data, transactionId) {
            let transaction = rootObject.d.getTransaction(transactionId)
            if (transaction && transaction.type === "library") {
                let library = OTL.Application.libraryFromData(data)
                if (library) {
                    let window = rootObject.getActiveWindow()
                    window.deepLinkHandler.openLibrary(transaction.url, library)
                } else {
                    console.error("Failed to create library from cache data")
                }
            }
        }

        function onLibraryNotFound(uid, transactionId) {
            rootObject.d.getTransaction(transactionId)
            let window = rootObject.getActiveWindow()
            window.deepLinkHandler.warnLinkTargetNotFound()
        }

        function onItemLoaded(uid, data, parents, library, transactionId) {
            let transaction = rootObject.d.getTransaction(transactionId)
            if (transaction && transaction.type === "item") {
                let window = rootObject.getActiveWindow()
                let lib = OTL.Application.libraryFromData(library)
                let item = OTL.Application.itemFromData(data)
                switch (item.itemType) {
                case "Note":
                    window.deepLinkHandler.openNote(transaction.url, item, lib)
                    break
                case "TodoList":
                    window.deepLinkHandler.openTodoList(transaction.url,
                                                        item, lib)
                    break
                case "Todo":
                    window.deepLinkHandler.openTodo(
                                transaction.url, item,
                                OTL.Application.itemFromData(parents[0]), lib)
                    break
                case "Image":
                    window.deepLinkHandler.openImage(transaction.url, item, lib)
                    break
                default:
                    console.error("Unhandled item type:", item.itemType)
                    window.deepLinkHandler.warnLinkTargetNotFound()
                }
            }
        }

        function onItemNotFound(uid, transactionId) {
            rootObject.d.getTransaction(transactionId)
            let window = rootObject.getActiveWindow()
            window.deepLinkHandler.warnLinkTargetNotFound()
        }
    }

    property var d: QtObject {
        property var transactions: ([])

        function addTransaction(transaction) {
            transactions.push(transaction)
        }

        function getTransaction(transactionId) {
            let result = null
            let remainingTransactions = []
            for (var i = 0; i < transactions.length; ++i) {
                let transaction = transactions[i]
                if (transaction.transactionId === transactionId) {
                    result = transaction
                } else {
                    remainingTransactions.push(transaction)
                }
            }
            transactions = remainingTransactions
            return result
        }
    }
}
