import QtQuick 2.10
import QtQuick.Layouts 1.3

import OpenTodoList 1.0 as OTL

import "../Components"
import "../Controls" as C
import "../Menues"
import "../Fonts"
import "../Utils"
import "../Widgets"
import "../Windows"

C.Page {
    id: page

    property OTL.Library library: null
    property bool syncRunning: {
        if (library == null) {
            // If we view all libraries, show sync indicator if there is any
            // sync running:
            return OTL.Application.directoriesWithRunningSync.length > 0
        } else {
            // Otherwise, only show indicator if the selected library syncs:
            return OTL.Application.directoriesWithRunningSync.indexOf(
                        library.directory) >= 0
        }
    }
    property int syncProgress: {
        let result = -1
        if (library) {
            // Only show concrete progress if we view a single library:
            result = OTL.Application.syncProgress[library.directory]
            if (result === undefined) {
                result = -1
            }
        }
        return result
    }

    property alias pageActions: libraryActions.actions
    property var undo: {
        if (d.savedItemStates.length > 0) {
            return function () {
                let list = d.savedItemStates.slice()
                OTL.Application.restoreItem(list.pop())
                d.savedItemStates = list
            }
        } else {
            return null
        }
    }

    signal openPage(var component, var properties)

    title: library ? library.name : qsTr("Schedule")

    Component {
        id: notePage
        NotePage {
            library: page.library
        }
    }

    Component {
        id: todoListPage
        TodoListPage {
            library: page.library
        }
    }

    Component {
        id: imagePage
        ImagePage {
            library: page.library
        }
    }

    Component {
        id: todoPage
        TodoPage {
            library: page.library
        }
    }

    LibraryPageActions {
        id: libraryActions

        library: page.library
        syncNowFunction: function () {
            d.triggerRefresh()
        }
        onOpenPage: page.openPage(component, properties)
    }

    C.ItemDelegate {
        id: sampleItemDelegate
        visible: false
    }

    Timer {
        interval: 1000 * 10 // Update every 10s - note due to value checking, mostly does nothing
        repeat: true
        running: true
        onTriggered: d.updateTimes()
    }

    Timer {
        interval: 0
        repeat: false
        running: true
        onTriggered: d.updateTimes()
    }

    OTL.ItemsModel {
        id: items
        onlyWithDueDate: true
        onlyUndone: true
        parentItem: page.library ? page.library.uid : ""
        overdueLabel: qsTr("Overdue")
        recursive: true
        cache: OTL.Application.cache
    }

    OTL.ItemsSortFilterModel {
        id: sortedItems
        sourceModel: items
        sortRole: OTL.ItemsModel.EffectiveDueToRole
    }

    QtObject {
        id: d

        property int hasScheduledItems: items.count > 0
        property var locale: Qt.locale()
        property var savedItemStates: []

        function d2s(date) {
            return date.toLocaleDateString(locale, "yyyy-MM-dd")
        }

        readonly property string todayLabel: qsTr("Today")
        readonly property string tomorrowLabel: qsTr("Tomorrow")

        function updateTimes() {
            var now = new Date()
            var today = new Date(now.getFullYear(), now.getMonth(),
                                 now.getDate())
            var tomorrow = new Date(now.getFullYear(), now.getMonth(),
                                    now.getDate() + 1)
            var laterThisWeek = new Date(now.getFullYear(), now.getMonth(),
                                         now.getDate() + 2)

            var lastDayOfWeek = (locale.firstDayOfWeek + 6) % 7
            var offsetToEndOfWeek = (lastDayOfWeek - now.getDay() + 7) % 7

            var timeOfOneDay = 24 * 60 * 60 * 1000

            var nextWeek = new Date(today.getTime() + timeOfOneDay)
            while (nextWeek.getDay() !== locale.firstDayOfWeek) {
                nextWeek = new Date(nextWeek.getTime() + timeOfOneDay)
            }

            var comingNext = new Date(nextWeek.getTime() + timeOfOneDay * 7)

            // If tomorrow is already the first day of the next week,
            // we "correct" the next week start date to the day after:
            if (d2s(nextWeek) <= d2s(tomorrow)) {
                nextWeek = new Date(nextWeek.getTime() + timeOfOneDay)
            }

            var timeSpans = {}

            timeSpans[d2s(today)] = todayLabel
            timeSpans[d2s(tomorrow)] = tomorrowLabel
            if (d2s(laterThisWeek) < d2s(nextWeek)) {
                // Only addd this, if the day after tomorrow is not already
                // in the next week.
                timeSpans[d2s(laterThisWeek)] = qsTr("Later This Week")
            }
            timeSpans[d2s(nextWeek)] = qsTr("Next Week")
            timeSpans[d2s(comingNext)] = qsTr("Coming Next")
            items.timeSpans = timeSpans
        }

        function openItemInPage(item) {
            let lib = page.library
            if (!lib) {

                // We are showing multiple libraries - select the proper one
            }

            switch (item.itemType) {
            case "Note":
                page.openPage(notePage, {
                                  "item": item,
                                  "library": lookupParent(libraries.libraries,
                                                          item.parents)
                              })
                break
            case "TodoList":
                page.openPage(todoListPage, {
                                  "item": item,
                                  "library": lookupParent(libraries.libraries,
                                                          item.parents)
                              })
                break
            case "Todo":
                page.openPage(todoPage, {
                                  "item": item,
                                  "library": lookupParent(libraries.libraries,
                                                          item.parents)
                              })
                break
            case "Image":
                page.openPage(imagePage, {
                                  "item": item,
                                  "library": lookupParent(libraries.libraries,
                                                          item.parents)
                              })
                break
            default:
                console.warn("Unhandled item type: " + item.itemType)
                break
            }
        }

        // Given a list of libraries and a list of parent uids, return the library
        // the item belongs to.
        function lookupParent(libraries, parents) {
            if (page.library != null) {
                return page.library
            }

            if (parents.length > 0) {
                let libUid = parents[0]
                for (var i = 0; i < libraries.length; ++i) {
                    let lib = libraries[i]
                    if (lib.uid === libUid) {
                        return lib
                    }
                }
            }
            return null
        }

        // Triggers a refresh.
        function triggerRefresh() {
            if (page.library) {
                OTL.Application.syncLibrary(page.library)
            } else {
                let libs = libraries.libraries
                for (var i = 0; i < libs.length; ++i) {
                    OTL.Application.syncLibrary(libs[i])
                }
            }
        }
    }

    BackgroundLabel {
        text: Markdown.stylesheet + qsTr(
                  "Nothing scheduled... Add a due date to items for them to " + "appear here.")
        visible: !d.hasScheduledItems
    }

    TodosWidget {
        id: listView
        anchors.fill: parent
        header: null
        allowSorting: false
        showParentItemInformation: true
        colorSwatchesVisible: page.library == null
        hideDueToLabelForSectionsFunction: function (section) {
            return section === d.todayLabel || section === d.tomorrowLabel
        }
        library: page.library
        libraryLookupFunction: function (item, library) {
            if (!library) {
                return d.lookupParent(libraries.libraries, item.parents)
            } else {
                return library
            }
        }

        onTodoClicked: {
            let clone = OTL.Application.cloneItem(todo)
            d.openItemInPage(clone)
        }
        onItemSaved: {
            let list = d.savedItemStates.slice()
            list.push(itemData)
            d.savedItemStates = list
        }
        itemsModel: sortedItems
        section {
            property: "effectiveDueToSpan"
            delegate: Heading {
                font.bold: true
                text: section
                width: parent.width
                padding: AppSettings.mediumSpace
            }
        }
    }

    PullToRefreshOverlay {
        anchors.fill: listView
        refreshEnabled: page.library == null || page.library.hasSynchronizer
        flickable: listView
        onRefresh: d.triggerRefresh()
    }

    SyncErrorNotificationBar {
        readonly property var syncErrors: {
            if (page.library) {
                return OTL.Application.syncErrors[page.library.directory] || []
            } else {
                return []
            }
        }

        library: page.library
        onShowErrors: page.openPage(syncErrorPage, {
                                        "errors": syncErrors
                                    })
    }

    NewTopLevelItemButton {
        createTodos: true
        createImages: false

        onNewTodoList: newItemDialog.createTodoList()
        onNewTodo: newItemDialog.createTodo()
        onNewNote: newItemDialog.createNote()
    }

    ItemCreatedNotification {
        id: itemCreatedNotification

        onOpen: d.openItemInPage(item)
    }

    NewItemWithDueDateDialog {
        id: newItemDialog

        library: page.library
        onAccepted: {
            let args = {
                "title": newItemTitle,
                "dueTo": newItemDueOn()
            }
            switch (itemType) {
            case "TodoList":
                var newItem = OTL.Application.addTodoList(selectedLibrary, args)
                break
            case "Todo":
                newItem = OTL.Application.addTodo(selectedLibrary,
                                                  parentItem, args)
                break
            case "Note":
                newItem = OTL.Application.addNote(selectedLibrary, args)
                break
            }
            if (newItem) {
                itemCreatedNotification.show(newItem)
            }
        }
    }

    OTL.LibrariesModel {
        id: libraries
        cache: page.library == null ? OTL.Application.cache : null
    }

    Component {
        id: syncErrorPage

        SyncErrorViewPage {}
    }
}
