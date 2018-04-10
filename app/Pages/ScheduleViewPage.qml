import QtQuick 2.10
import QtQuick.Layouts 1.3

import OpenTodoList 1.0 as OTL

import "../Components"
import "../Menues"
import "../Fonts"
import "../Utils"
import "../Widgets"

Page {
    id: page

    property OTL.Library library: null
    property Menu pageMenu: LibraryPageMenu {
        x: page.width
        library: page.library
        onOpenPage: page.openPage(component, properties)
    }

    signal openPage(var component, var properties)

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

    QtObject {
        id: d

        property int hasScheduledItems: (overdueTodos.count +
                                         overdueTopLevelItems.count +
                                         todaysTodos.count +
                                         todaysTopLevelItems.count +
                                         thisWeeksTodos.count +
                                         thisWeeksTopLevelItems.count +
                                         nextWeeksTodos.count +
                                         nextWeeksTopLevelItems.count +
                                         upcomingTodos.count +
                                         upcomingTopLevelItems.count) > 0

        function updateTimes() {
            var today = new Date();
            var startOfToday = new Date(today.getFullYear(),
                                        today.getMonth(),
                                        today.getDate());
            var endOfToday = new Date(today.getFullYear(),
                                      today.getMonth(),
                                      today.getDate() + 1);

            var locale = Qt.locale();
            var lastDayOfWeek = (locale.firstDayOfWeek + 6) % 7;
            var offsetToEndOfWeek = (lastDayOfWeek - today.getDay() + 7) % 7;

            var endOfWeek = new Date(startOfToday.getTime() +
                                     offsetToEndOfWeek * 24 * 60 * 60 * 1000 + 1);
            var endOfNextWeek = new Date(endOfWeek.getTime() +
                                         7 * 25 * 60 * 60 * 1000);

            overdueTodos.maxDueDate = startOfToday;
            overdueTopLevelItems.maxDueDate = startOfToday;

            todaysTodos.minDueDate = startOfToday;
            todaysTodos.maxDueDate = endOfToday;
            todaysTopLevelItems.minDueDate = startOfToday;
            todaysTopLevelItems.maxDueDate = endOfToday;

            thisWeeksTodos.minDueDate = endOfToday;
            thisWeeksTodos.maxDueDate = endOfWeek;
            thisWeeksTopLevelItems.minDueDate = endOfToday;
            thisWeeksTopLevelItems.maxDueDate = endOfWeek;

            nextWeeksTodos.minDueDate = endOfWeek;
            nextWeeksTodos.maxDueDate = endOfNextWeek;
            nextWeeksTopLevelItems.minDueDate = endOfWeek;
            nextWeeksTopLevelItems.maxDueDate = endOfNextWeek;

            upcomingTodos.minDueDate = endOfNextWeek;
            upcomingTopLevelItems.minDueDate = endOfNextWeek;

            console.debug("Start of today: " + startOfToday);
            console.debug("End of today: " + endOfToday);
            console.debug("End of this week: " + endOfWeek);
            console.debug("End of next week: " + endOfNextWeek);
        }
    }

    Component {
        id: itemDelegate

        ItemDelegate {
            width: parent.width
            onClicked: {
                switch (object.itemType) {
                case "Note":
                    page.openPage(notePage, { item: object });
                    break;
                case "TodoList":
                    page.openPage(todoListPage, { item: object });
                    break;
                case "Todo":
                    page.openPage(todoPage, { item: object });
                    break;
                case "Image":
                    page.openPage(imagePage, { item: object });
                    break;
                default:
                    console.warn("Unhandled item type: " + object.itemType);
                    break;
                }
            }

            contentItem: RowLayout {
                width: parent.width

                ToolButton {
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
                    onClicked: {
                        switch (object.itemType) {
                        case "Todo":
                            object.done = !object.done;
                            break;
                        default:
                            break;
                        }
                    }
                }
                Label {
                    Layout.fillWidth: true
                    text: Markdown.format(object.title)
                }
            }
        }
    }

    BackgroundSymbol {
        symbol: Icons.faClock
    }

    BackgroundLabel {
        text: qsTr("Nothing scheduled... Add a due date to items for them to " +
                   "appear here.")
        visible: !d.hasScheduledItems
    }

    OTL.ItemsModel {
        id: topLevelItems
        container: page.library.topLevelItems
    }

    OTL.ItemsModel {
        id: todos
        container: page.library.todos
    }

    Pane {
        anchors.fill: parent

        ScrollView {
            id: scrollView
            anchors.fill: parent

            Column {
                width: scrollView.width

                Label {
                    font.bold: true
                    text: qsTr("Overdue")
                    width: parent.width
                    visible: overdueTodos.count + overdueTopLevelItems.count > 0
                    topPadding: 20
                }

                Repeater {
                    model: OTL.ItemsSortFilterModel {
                        id: overdueTopLevelItems
                        sourceModel: topLevelItems
                    }
                    delegate: itemDelegate
                }

                Repeater {
                    model: OTL.ItemsSortFilterModel {
                        id: overdueTodos
                        sourceModel: todos
                        onlyUndone: true
                    }
                    delegate: itemDelegate
                }

                Label {
                    font.bold: true
                    text: qsTr("Today")
                    width: parent.width
                    visible: todaysTodos.count + todaysTopLevelItems.count > 0
                    topPadding: 20
                }

                Repeater {
                    model: OTL.ItemsSortFilterModel {
                        id: todaysTopLevelItems
                        sourceModel: topLevelItems
                    }
                    delegate: itemDelegate
                }

                Repeater {
                    model: OTL.ItemsSortFilterModel {
                        id: todaysTodos
                        sourceModel: todos
                        onlyUndone: true
                    }

                    delegate: itemDelegate
                }

                Label {
                    font.bold: true
                    text: qsTr("Later This Week")
                    width: parent.width
                    visible: thisWeeksTodos.count + thisWeeksTopLevelItems.count > 0
                    topPadding: 20
                }

                Repeater {
                    model: OTL.ItemsSortFilterModel {
                        id: thisWeeksTopLevelItems
                        sourceModel: topLevelItems
                    }
                    delegate: itemDelegate
                }

                Repeater {
                    model: OTL.ItemsSortFilterModel {
                        id: thisWeeksTodos
                        sourceModel: todos
                        onlyUndone: true
                    }

                    delegate: itemDelegate
                }

                Label {
                    font.bold: true
                    text: qsTr("Next Week")
                    width: parent.width
                    visible: nextWeeksTodos.count + nextWeeksTopLevelItems.count > 0
                    topPadding: 20
                }

                Repeater {
                    model: OTL.ItemsSortFilterModel {
                        id: nextWeeksTopLevelItems
                        sourceModel: topLevelItems
                    }
                    delegate: itemDelegate
                }

                Repeater {
                    model: OTL.ItemsSortFilterModel {
                        id: nextWeeksTodos
                        sourceModel: todos
                        onlyUndone: true
                    }

                    delegate: itemDelegate
                }

                Label {
                    font.bold: true
                    text: qsTr("Coming Next")
                    width: parent.width
                    visible: upcomingTodos.count + upcomingTopLevelItems.count > 0
                    topPadding: 20
                }

                Repeater {
                    model: OTL.ItemsSortFilterModel {
                        id: upcomingTopLevelItems
                        sourceModel: topLevelItems
                    }
                    delegate: itemDelegate
                }

                Repeater {
                    model: OTL.ItemsSortFilterModel {
                        id: upcomingTodos
                        sourceModel: todos
                        onlyUndone: true
                    }

                    delegate: itemDelegate
                }
            }
        }
    }

    SyncIndicatorBar {
        id: syncIndicatorBar
        library: page.library
    }

    LibrarySecretsMissingNotificationBar { library: page.library }

    SyncErrorNotificationBar {
        library: page.library
        onShowErrors: page.openPage(syncErrorPage, { errors: page.library.syncErrors })
    }

    Component {
        id: syncErrorPage

        SyncErrorViewPage {}
    }

}
