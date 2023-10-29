import QtQuick 2.10
import QtQuick.Controls 2.3 as QQC2
import QtCore as QtCore
import OpenTodoList as OTL

QQC2.StackView {
    id: stack


    /**
     * @brief A unique ID used to store the persistent data of the page stack.
     */
    property string stackId: ""


    /**
     * @brief If set, when upon restoring the stack is empty, create an instance of this component.
     */
    property var startPageComponent


    /**
     * @brief Indicates if a sync is running.
     */
    property bool syncRunning: {
        let syncRunning = false
        d.forAllPages(stack.depth, page => {
                          if (page.syncRunning !== undefined) {
                              syncRunning = page.syncRunning
                              return false
                          }
                          return true
                      })
        return syncRunning
    }


    /**
     * @brief The process of a sync operation that is running.
     */
    property int syncProgress: {
        let result = -1
        d.forAllPages(stack.depth, page => {
                          if (typeof (page.syncProgress) === "number") {
                              result = page.syncProgress
                              return false
                          }
                          return true
                      })
        return result
    }


    /**
     * @brief The topmost item which is attached to a page.
     */
    property OTL.LibraryItem topmostItem: {
        let result = null
        d.forAllPages(stack.depth, page => {
                          if (page.item instanceof OTL.LibraryItem) {
                              result = page.item
                              return false
                          }
                          return true
                      })
        return result
    }


    /**
     * @brief The topmost todo which is attached to a page.
     */
    property OTL.Todo topmostTodo: {
        let result = null
        d.forAllPages(stack.depth, page => {
                          if (page.item instanceof OTL.Todo) {
                              result = page.item
                              return false
                          }
                          return true
                      })
        return result
    }


    /**
     * @brief The topmost item which is attached to a page.
     */
    property OTL.TodoList topmostTodoList: {
        let result = null
        d.forAllPages(stack.depth, page => {
                          if (page.item instanceof OTL.TodoList) {
                              result = page.item
                              return false
                          }
                          return true
                      })
        return result
    }


    /**
     * @brief The topmost library which is attached to a page.
     */
    property OTL.Library topmostLibrary: {
        let result = null
        d.forAllPages(stack.depth, page => {
                          if (page.library instanceof OTL.Library) {
                              result = page.library
                              return false
                          }
                          return true
                      })
        return result
    }

    property var topmostPageWithSelectColorFunction: {
        let result = null
        d.forAllPages(stack.depth, page => {
                          if (typeof (page.selectColor) === "function") {
                              result = page
                              return false
                          }
                          return true
                      })
        return result
    }


    /**
     * @brief Indicates if there is a page on the stack that can be popped off.
     */
    property bool canGoBack: currentItem != null
                             && (depth > 1
                                 || typeof (currentItem.goBack) === "function")


    /**
     * @brief Go back in the stack hierarchy.
     *
     * If page is a page in the stack, pop all pages off the stack up to that page.
     * Otherwise, pop off the top most page. If the top most page has a goBack function, this
     * function is called. Otherwise, the top most page is popped off as-is.
     */
    function goBack(page) {
        if (page !== undefined && page !== null) {
            stackView.pop(page)
        } else if (typeof (currentItem.goBack) === "function") {
            currentItem.goBack()
        } else {
            stackView.pop()
        }
    }

    function clearAndOpenPage(url) {
        clear()
        push(url)
    }

    QtObject {
        id: d

        property bool initialized: false


        /**
         * @brief Iterate over all pages and call a function on each one.
         *
         * This iterates over all pages, from the top most one to the lower most one. The function
         * fn is called for each page (if the page is loaded). The function shall return a boolean
         * value indicating if iteration should continue (true) or stop (false).
         */
        function forAllPages(depth, fn) {
            for (var i = depth - 1; i >= 0; --i) {
                let page = stack.get(i)
                if (page) {
                    if (!fn(page)) {
                        return
                    }
                }
            }
        }
    }

    Connections {
        target: stack
        function onCurrentItemChanged() {
            if (d.initialized && stackId != "") {
                let state = []
                for (var i = 0; i < stack.depth; ++i) {
                    let page = stack.get(i)
                    let pageUrl = page.restoreUrl
                    if (pageUrl && pageUrl.toString() !== "") {
                        state.push({
                                       "url": pageUrl,
                                       "state": page.savePage()
                                   })
                    } else {
                        break
                    }
                }
                settings.setValue("state", JSON.stringify(state))
            }
        }
    }

    Component.onCompleted: {
        try {
            if (stack.stackId !== "") {
                let state = null
                try {
                    state = JSON.parse(settings.value("state", "[]"))
                } catch (exc) {
                    console.warn("Exception caught during stack state restoration:",
                                 exc)
                    state = []
                }
                for (var i = 0; i < state.length; ++i) {
                    let pageState = state[i]
                    let page = stack.push(Qt.resolvedUrl(pageState.url))
                    page.restorePage(pageState.state)
                }
            }
        } catch (e) {
            console.warn("Error during restoring stack: ", e)
        }
        try {
            if (stack.depth === 0 && startPageComponent) {
                stack.push(startPageComponent)
            }
        } catch (e2) {
            console.warn("Error during creating start page: ", e2)
        }

        d.initialized = true
    }

    QtCore.Settings {
        id: settings
        category: "StackView"
    }
}
