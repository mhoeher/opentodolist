import QtQuick 2.5
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3

import OpenTodoList 1.0
import OpenTodoList.UI 1.0

Page {
    id: page

    property Library library: null

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

        Rectangle {
            width: parent.width
            height: Math.max(row.height, Globals.minButtonHeight)
            color: Colors.light
            border {
                width: 1
                color: Colors.midlight
            }

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    switch (object.itemType) {
                    case "Note":
                        page.openPage(notePage, { item: object });
                        break;
                    case "TodoList":
                        page.openPage(todoListPage, { item: object });
                        break;
                    case "Todo":
                        page.openPage(todoPage, { todo: object });
                        break;
                    case "Image":
                        page.openPage(imagePage, { item: object });
                        break;
                    default:
                        console.warn("Unhandled item type: " + object.itemType);
                        break;
                    }
                }
            }

            RowLayout {
                id: row

                anchors {
                    left: parent.left
                    right: parent.right
                    margins: Globals.defaultMargin
                    verticalCenter: parent.verticalCenter
                }

                Label {
                    font.family: Fonts.symbols.name
                    text: {
                        switch (object.itemType) {
                        case "Todo":
                            return object.done ? Fonts.symbols.faCheckSquareO :
                                                 Fonts.symbols.faSquareO;
                        case "TodoList":
                            return Fonts.symbols.faList;
                        case "Note":
                            return Fonts.symbols.faStickyNoteO;
                        case "Image":
                            return Fonts.symbols.faPictureO;
                        default:
                            return Fonts.symbols.faQuestionCircleO;
                        }
                    }

                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            if (typeof(object.done) === "boolean") {
                                object.done = !object.done;
                            }
                        }
                    }
                }

                Label {
                    wrapMode: Text.WordWrap
                    text: object.title
                    Layout.fillWidth: true
                }
            }
        }
    }

    BackgroundSymbol {
        symbol: Fonts.symbols.faClockO
    }

    BackgroundLabel {
        text: qsTr("Nothing scheduled... Add a due date to items for them to " +
                   "appear here.")
        visible: !d.hasScheduledItems
    }

    ItemsModel {
        id: topLevelItems
        container: page.library.topLevelItems
    }

    ItemsModel {
        id: todos
        container: page.library.todos
    }

    ScrollView {
        anchors.fill: parent

        Column {
            width: page.width - 2 * Globals.defaultMargin
            x: Globals.defaultMargin
            spacing: Globals.defaultMargin / 2

            Item {
                height: Globals.defaultMargin
                width: 1
            }

            Label {
                font.pixelSize: Globals.fontPixelSize * 2
                font.bold: true
                text: qsTr("Overdue")
                width: parent.width
                wrapMode: Text.WordWrap
                visible: overdueTodos.count + overdueTopLevelItems.count > 0
            }

            Repeater {
                model: ItemsSortFilterModel {
                    id: overdueTopLevelItems
                    sourceModel: topLevelItems
                }
                delegate: itemDelegate
            }

            Repeater {
                model: ItemsSortFilterModel {
                    id: overdueTodos
                    sourceModel: todos
                    onlyUndone: true
                }

                delegate: itemDelegate
            }

            Label {
                font.pixelSize: Globals.fontPixelSize * 2
                font.bold: true
                text: qsTr("Today")
                width: parent.width
                wrapMode: Text.WordWrap
                visible: todaysTodos.count + todaysTopLevelItems.count > 0
            }

            Repeater {
                model: ItemsSortFilterModel {
                    id: todaysTopLevelItems
                    sourceModel: topLevelItems
                }
                delegate: itemDelegate
            }

            Repeater {
                model: ItemsSortFilterModel {
                    id: todaysTodos
                    sourceModel: todos
                    onlyUndone: true
                }

                delegate: itemDelegate
            }

            Label {
                font.pixelSize: Globals.fontPixelSize * 2
                font.bold: true
                text: qsTr("Later This Week")
                width: parent.width
                wrapMode: Text.WordWrap
                visible: thisWeeksTodos.count + thisWeeksTopLevelItems.count > 0
            }

            Repeater {
                model: ItemsSortFilterModel {
                    id: thisWeeksTopLevelItems
                    sourceModel: topLevelItems
                }
                delegate: itemDelegate
            }

            Repeater {
                model: ItemsSortFilterModel {
                    id: thisWeeksTodos
                    sourceModel: todos
                    onlyUndone: true
                }

                delegate: itemDelegate
            }

            Label {
                font.pixelSize: Globals.fontPixelSize * 2
                font.bold: true
                text: qsTr("Next Week")
                width: parent.width
                wrapMode: Text.WordWrap
                visible: nextWeeksTodos.count + nextWeeksTopLevelItems.count > 0
            }

            Repeater {
                model: ItemsSortFilterModel {
                    id: nextWeeksTopLevelItems
                    sourceModel: topLevelItems
                }
                delegate: itemDelegate
            }

            Repeater {
                model: ItemsSortFilterModel {
                    id: nextWeeksTodos
                    sourceModel: todos
                    onlyUndone: true
                }

                delegate: itemDelegate
            }

            Label {
                font.pixelSize: Globals.fontPixelSize * 2
                font.bold: true
                text: qsTr("Coming Next")
                width: parent.width
                wrapMode: Text.WordWrap
                visible: upcomingTodos.count + upcomingTopLevelItems.count > 0
            }

            Repeater {
                model: ItemsSortFilterModel {
                    id: upcomingTopLevelItems
                    sourceModel: topLevelItems
                }
                delegate: itemDelegate
            }

            Repeater {
                model: ItemsSortFilterModel {
                    id: upcomingTodos
                    sourceModel: todos
                    onlyUndone: true
                }

                delegate: itemDelegate
            }

            Item {
                height: Globals.defaultMargin
                width: 1
            }
        }
    }
}
