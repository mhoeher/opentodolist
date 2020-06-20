import QtQuick 2.10
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.12

import OpenTodoList 1.0 as OTL

import "../Components"
import "../Menues"
import "../Fonts"
import "../Utils"
import "../Widgets"
import "../Windows"

Page {
    id: page

    property OTL.Library library: null
    property bool syncRunning: {
        return library &&
                OTL.Application.directoriesWithRunningSync.indexOf(
                    library.directory) >= 0;
    }
    property alias pageActions: libraryActions.actions

    signal openPage(var component, var properties)

    title: library.name

    Component {
        id: notePage
        NotePage { library: page.library }
    }

    Component {
        id: todoListPage
        TodoListPage { library: page.library }
    }

    Component {
        id: imagePage
        ImagePage { library: page.library }
    }

    Component {
        id: todoPage
        TodoPage { library: page.library }
    }

    LibraryPageActions {
        id: libraryActions

        library: page.library
        onOpenPage: page.openPage(component, properties)
    }

    ItemDelegate {
        id: sampleItemDelegate
        visible: false
    }

    Timer {
        interval: 1000 * 60 * 60
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
        parentItem: page.library.uid
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

        function d2s(date) {
            return date.toLocaleDateString(locale, "yyyy-MM-dd");
        }

        readonly property string todayLabel: qsTr("Today")
        readonly property string tomorrowLabel: qsTr("Tomorrow")

        function updateTimes() {
            var now = new Date();
            var today = new Date(now.getFullYear(),
                                 now.getMonth(),
                                 now.getDate());
            var tomorrow = new Date(now.getFullYear(),
                                    now.getMonth(),
                                    now.getDate() + 1);
            var laterThisWeek = new Date(now.getFullYear(),
                                         now.getMonth(),
                                         now.getDate() + 2);

            var lastDayOfWeek = (locale.firstDayOfWeek + 6) % 7;
            var offsetToEndOfWeek = (lastDayOfWeek - now.getDay() + 7) % 7;

            var timeOfOneDay = 24 * 60 * 60 * 1000;

            var nextWeek = new Date(today.getTime() +
                                    timeOfOneDay);
            while (nextWeek.getDay() !== locale.firstDayOfWeek) {
                nextWeek = new Date(nextWeek.getTime() + timeOfOneDay);
            }

            var comingNext = new Date(nextWeek.getTime() +
                                      timeOfOneDay * 7);

            // If tomorrow is already the first day of the next week,
            // we "correct" the next week start date to the day after:
            if (d2s(nextWeek) <= d2s(tomorrow)) {
                nextWeek = new Date(nextWeek.getTime() + timeOfOneDay);
            }


            var timeSpans = {};

            timeSpans[d2s(today)] = todayLabel;
            timeSpans[d2s(tomorrow)] = tomorrowLabel;
            if (d2s(laterThisWeek) < d2s(nextWeek)) {
                // Only addd this, if the day after tomorrow is not already
                // in the next week.
                timeSpans[d2s(laterThisWeek)] = qsTr("Later This Week");
            }
            timeSpans[d2s(nextWeek)] = qsTr("Next Week");
            timeSpans[d2s(comingNext)] = qsTr("Coming Next");
            items.timeSpans = timeSpans;
        }

        function openItemInPage(item) {
            switch (item.itemType) {
            case "Note":
                page.openPage(notePage, { item: item });
                break;
            case "TodoList":
                page.openPage(todoListPage, { item: item });
                break;
            case "Todo":
                page.openPage(todoPage, { item: item });
                break;
            case "Image":
                page.openPage(imagePage, { item: item });
                break;
            default:
                console.warn("Unhandled item type: " + item.itemType);
                break;
            }
        }
    }

    Component {
        id: itemDelegate

        Loader {
            asynchronous: true
            width: parent.width
            height: itemDelegateInner.height

            ItemDelegate {
                id: itemDelegateInner

                width: parent.width

                onClicked: d.openItemInPage(object)
                contentItem: Item {
                    id: cntItem

                    function updateImplicitHeight() {
                        updateImplicitHeightTimer.restart();
                    }

                    width: parent.width
                    implicitHeight: sampleItemDelegate.implicitHeight

                    Timer {
                        id: updateImplicitHeightTimer
                        interval: 100
                        repeat: false
                        onTriggered: cntItem.implicitHeight = Math.max(
                                         itemSymbol.height,
                                         itemTitle.contentHeight)
                    }

                    ToolButton {
                        id: itemSymbol

                        background: Item {}
                        symbol: {
                            switch (object.itemType) {
                            case "Todo":
                                return object.done ? Icons.faCheckCircle:
                                                     Icons.faCircle;
                            case "TodoList":
                                return Icons.faListAlt;
                            case "Note":
                                return Icons.faStickyNote;
                            case "Image":
                                return Icons.faImage;
                            default:
                                return Icons.faQuestionCircle
                            }
                        }
                        font.family: Fonts.icons
                        onClicked: object.markCurrentOccurrenceAsDone()
                        anchors {
                            left: parent.left
                            verticalCenter: parent.verticalCenter
                        }
                        onHeightChanged: cntItem.updateImplicitHeight()
                    }
                    Label {
                        id: itemTitle

                        text: Markdown.markdownToHtml(object.title)
                        textFormat: Text.RichText
                        anchors {
                            left: itemSymbol.right
                            right: parent.right
                            verticalCenter: parent.verticalCenter
                        }
                        onContentHeightChanged: cntItem.updateImplicitHeight()
                    }
                }

                ProgressItemOverlay {
                    item: object
                    height: itemDelegateInner.height
                }
            }
        }
    }

    BackgroundLabel {
        text: Markdown.stylesheet +
              qsTr("Nothing scheduled... Add a due date to items for them to " +
                   "appear here.")
        visible: !d.hasScheduledItems
    }

    TodosWidget {
        id: listView
        anchors.fill: parent
        header: null
        allowSorting: false
        hideDueToLabelForSectionsFunction: function(section) {
            return section === d.todayLabel || section === d.tomorrowLabel;
        }

        onTodoClicked: {
            switch (todo.itemType) {
            case "Note":
                page.openPage(notePage, { item: todo });
                break;
            case "TodoList":
                page.openPage(todoListPage, { item: todo });
                break;
            case "Todo":
                page.openPage(todoPage, { item: todo });
                break;
            case "Image":
                page.openPage(imagePage, { item: todo });
                break;
            default:
                console.warn("Unhandled item type: " + todo.itemType);
                break;
            }
        }
        model: sortedItems
        section {
            property: "effectiveDueToSpan"
            delegate: Heading {
                font.bold: true
                text: section
                width: parent.width
                padding: 10
            }
        }
    }

    PullToRefreshOverlay {
        anchors.fill: listView
        refreshEnabled: page.library.hasSynchronizer
        flickable: listView
        onRefresh: OTL.Application.syncLibrary(page.library)
    }

    SyncErrorNotificationBar {
        readonly property var syncErrors: {
            if (page.library) {
                return OTL.Application.syncErrors[page.library.directory] || [];
            } else {
                return [];
            }
        }

        library: page.library
        onShowErrors: page.openPage(syncErrorPage,
                                    {
                                        errors: syncErrors
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
            let args = {title: newItemTitle, dueTo: newItemDueOn};
            switch (itemType) {
            case "TodoList":
                var newItem = OTL.Application.addTodoList(page.library, args);
                break;
            case "Todo":
                newItem = OTL.Application.addTodo(page.library, parentItem, args);
                break;
            case "Note":
                newItem = OTL.Application.addNote(page.library, args);
                break;
            }
            if (newItem) {
                itemCreatedNotification.show(newItem);
            }
        }
    }

    Component {
        id: syncErrorPage

        SyncErrorViewPage {}
    }
}
