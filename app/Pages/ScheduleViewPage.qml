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
        sortRole: OTL.ItemsModel.DueToRole
    }

    QtObject {
        id: d

        property int hasScheduledItems: items.count > 0

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

            var timeSpans = {};
            timeSpans[startOfToday.toISOString()] = "Today";
            timeSpans[endOfToday.toISOString()] = "Later This Week";
            timeSpans[endOfWeek.toISOString()] = "Next Week";
            timeSpans[endOfNextWeek.toISOString()] = "Coming Next";
            items.timeSpans = timeSpans;
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
                        onClicked: {
                            switch (object.itemType) {
                            case "Todo":
                                object.done = !object.done;
                                break;
                            default:
                                break;
                            }
                        }
                        anchors {
                            left: parent.left
                            verticalCenter: parent.verticalCenter
                        }
                        onHeightChanged: cntItem.updateImplicitHeight()
                    }
                    MarkdownLabel {
                        id: itemTitle

                        markdown: object.title
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

    BackgroundSymbol {
        symbol: Icons.faClock
    }

    BackgroundLabel {
        text: Markdown.stylesheet +
              qsTr("Nothing scheduled... Add a due date to items for them to " +
                   "appear here.")
        visible: !d.hasScheduledItems
    }

    ListView {
        anchors.fill: parent
        delegate: itemDelegate
        model: sortedItems
        section {
            property: "dueToSpan"
            delegate: Label {
                font.bold: true
                text: section
                width: parent.width
                height: 30
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
