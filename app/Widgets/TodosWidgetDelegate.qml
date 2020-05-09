import QtQuick 2.10
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.0

import OpenTodoList 1.0 as OTL

import "../Components" as Components
import "../Utils"
import "../Fonts"
import "../Windows"
import "../Actions" as Actions

SwipeDelegate {
    id: swipeDelegate
    
    property var model: null
    property bool toggleDoneOnClose: false
    property OTL.Item item: OTL.Item {}
    readonly property var itemActions: ([
                                            renameAction,
                                            setDueTodayAction,
                                            setDueTomorrowAction,
                                            setDueNextWeekAction,
                                            setDueThisWeekAction,
                                            setDueToAction,
                                            deleteAction
                                        ])
    property bool allowSorting: false

    signal itemPressedAndHold()
    signal itemClicked()
    
    width: parent.width
    padding: 0
    topPadding: 0
    bottomPadding: 0
    hoverEnabled: true
    contentItem: RowLayout {
        width: parent.width
        
        Components.ToolButton {
            font.family: Fonts.icons
            symbol: {
                switch (swipeDelegate.item.itemType) {
                case "Todo":
                case "Task":
                    return swipeDelegate.item.done ? Icons.faCheckCircle:
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
            onClicked: {
                switch (swipeDelegate.item.itemType) {
                case "Task":
                    swipeDelegate.item.done = !swipeDelegate.item.done;
                    break;
                case "Todo":
                case "TodoList":
                case "Note":
                case "Image":
                    swipeDelegate.item.markCurrentOccurrenceAsDone();
                    break;
                default:
                    break;
                }
            }
        }
        Column {
            Layout.fillWidth: true
            
            Components.Label {
                text: Markdown.markdownToHtml(swipeDelegate.item.title)
                textFormat: Text.RichText
                width: parent.width
            }
            Item {
                height: 10
                width: 1
                visible: swipeDelegate.item.effectiveDueTo !== undefined &&
                         DateUtils.validDate(swipeDelegate.item.effectiveDueTo)
            }
            RowLayout {
                width: parent.width
                visible: swipeDelegate.item.effectiveDueTo !== undefined &&
                         DateUtils.validDate(swipeDelegate.item.effectiveDueTo)
                opacity: 0.5
                
                Label {
                    font.family: Fonts.icons
                    text: Icons.faCalendarAlt
                }
                Label {
                    Layout.fillWidth: true
                    text: DateUtils.format(swipeDelegate.item.effectiveDueTo)
                }
            }
        }
        Item {
            visible: !toggleSwipeOpened.visible
            width: toggleSwipeOpened.width
            height: toggleSwipeOpened.height
        }
        
        Components.ToolButton {
            id: toggleSwipeOpened
            visible: swipeDelegate.hovered
            symbol: swipeDelegate.swipe.position === 0 ?
                        Icons.faChevronLeft :
                        Icons.faChevronRight
            onClicked: {
                if (swipeDelegate.swipe.position === 0) {
                    swipeDelegate.swipe.open(
                                SwipeDelegate.Right);
                } else {
                    swipeDelegate.swipe.close();
                }
            }
        }
    }
    swipe.right: Row {
        anchors.right: parent.right
        height: parent.height

        Components.ItemActionToolButtons {
            id: itemToolButtonActions

            model: swipeDelegate.itemActions
        }
    }
    swipe.left: Pane {
        height: swipeDelegate.height
        width: swipeDelegate.width / 2
        enabled: {
            let item = swipeDelegate.item;
            return item.done !== undefined || item.dueTo !== undefined;
        }

        Material.background: Colors.positiveColor
        
        Components.Label {
            text: {
                let item = swipeDelegate.item;
                let done = item.done;
                if (done === undefined) {
                    done = !DateUtils.validDate(item.dueTo);
                }
                if (done) {
                    return qsTr("Swipe to mark undone");
                } else {
                    return qsTr("Swipe to mark done");
                }
            }
            wrapMode: "WrapAtWordBoundaryOrAnywhere"
            width: parent.width / 2
            anchors.fill: parent
        }
    }
    
    onClicked: {
        // Delay evaluation, as it overlaps with double clicks:
        if (!delayedClickTimer.afterDoubleClick) {
            delayedClickTimer.start();
        }
        delayedClickTimer.afterDoubleClick = false;
    }
    
    onDoubleClicked: {
        // Remember that we detected a double click:
        delayedClickTimer.afterDoubleClick = true;
        
        // Handle the double click:
        delayedClickTimer.stop();
        renameAction.trigger();
    }
    
    swipe.onCompleted: {
        if (swipe.position > 0) {
            // Swipe from left to right to mark items as (un)done.
            swipeDelegate.toggleDoneOnClose = true;
            swipeDelegate.swipe.close();
        } else {
            d.openSwipeDelegate = swipeDelegate;
        }
    }
    
    onPressAndHold: {
        if (swipeDelegate.allowSorting) {
            d.openSwipeDelegate = null;
            reorderOverlay.startDrag();
        } else {
            swipeDelegate.itemPressedAndHold();
        }
    }
    swipe.onClosed: {
        if (swipeDelegate.toggleDoneOnClose) {
            if (typeof(swipeDelegate.item.markCurrentOccurrenceAsDone) === "function") {
                swipeDelegate.item.markCurrentOccurrenceAsDone();
            } else {
                swipeDelegate.item.done = !swipeDelegate.item.done;
            }
            swipeDelegate.toggleDoneOnClose = false;
        }
    }
    
    Connections {
        target: d
        onOpenSwipeDelegateChanged: if (d.openSwipeDelegate !==
                                            swipeDelegate) {
                                        swipeDelegate.swipe.close();
                                    }
    }
    
    ProgressItemOverlay {
        item: swipeDelegate.item
        height: background.height
        x: {
            if (swipeDelegate.swipe.rightItem) {
                return swipeDelegate.swipe.position *
                        swipeDelegate.swipe.rightItem.width;
            } else {
                return 0;
            }
        }
    }
    
    ReorderableListViewOverlay {
        id: reorderOverlay
        anchors.fill: parent
        model: swipeDelegate.model
    }

    Timer {
        id: delayedClickTimer

        property bool afterDoubleClick: false

        interval: 300
        repeat: false
        onTriggered: {
            d.openSwipeDelegate = null;
            swipeDelegate.itemClicked();
        }
    }

    Actions.RenameItem { id: renameAction; item: swipeDelegate.item }
    Actions.SetDueTo { id: setDueToAction; item: swipeDelegate.item }
    Actions.DeleteItem { id: deleteAction; item: swipeDelegate.item }
    Actions.SetDueToday { id: setDueTodayAction; item: swipeDelegate.item; hideButton: true }
    Actions.SetDueTomorrow { id: setDueTomorrowAction; item: swipeDelegate.item; hideButton: true }
    Actions.SetDueThisWeek { id: setDueThisWeekAction; item: swipeDelegate.item; hideButton: true }
    Actions.SetDueNextWeek { id: setDueNextWeekAction; item: swipeDelegate.item; hideButton: true }
}
