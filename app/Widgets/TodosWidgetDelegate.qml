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
    
    property bool showParentItemInformation: false
    property var model: null
    property bool toggleDoneOnClose: false
    property OTL.Item item: OTL.Item {}
    property bool hideDueDate: false
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
    property bool drawSeperator: true
    property ItemDragTile dragTile

    signal itemPressedAndHold()
    signal itemClicked()
    signal setSwipeDelegate(var item)
    
    width: parent ? parent.width : implicitWidth
    padding: 0
    topPadding: fontMetrics.height / 2
    bottomPadding: topPadding
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
                case "Todo":
                    swipeDelegate.item.done = !swipeDelegate.item.done;
                    break;
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
                height: fontMetrics.height / 4
                width: 1
                visible: itemInfoGrid.height > 0
            }
            GridLayout {
                id: itemInfoGrid
                width: parent.width
                opacity: 0.5
                columns: {
                    if (itemDueDateIcon.width + itemDueDateText.implicitWidth < width / 2) {
                        return 4;
                    } else {
                        return 2;
                    }
                }

                onColumnsChanged: forceLayout()

                Label {
                    font.family: Fonts.icons
                    text: Icons.faList
                    visible: d.parentItem
                }
                Label {
                    Layout.fillWidth: true
                    text: d.parentItem ? d.parentItem.title : ""
                    visible: d.parentItem
                }
                
                Label {
                    id: itemDueDateIcon
                    font.family: Fonts.icons
                    text: Icons.faCalendarAlt
                    visible: swipeDelegate.item.effectiveDueTo !== undefined &&
                             DateUtils.validDate(swipeDelegate.item.effectiveDueTo) &&
                             !swipeDelegate.hideDueDate
                }
                Label {
                    id: itemDueDateText
                    Layout.fillWidth: true
                    text: DateUtils.format(swipeDelegate.item.effectiveDueTo)
                    visible: itemDueDateIcon.visible
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
    
    onItemChanged: d.loadParentItem()
    onShowParentItemInformationChanged: d.loadParentItem()

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
            swipeDelegate.setSwipeDelegate(swipeDelegate);
        }
    }
    
    onPressAndHold: {
        if (swipeDelegate.allowSorting) {
            swipeDelegate.setSwipeDelegate(null);
            reorderOverlay.startDrag();
        } else {
            swipeDelegate.itemPressedAndHold();
        }
    }
    swipe.onClosed: {
        if (swipeDelegate.toggleDoneOnClose) {
            let item = swipeDelegate.item;
            switch (item.itemType) {
            case "Todo":
            case "Task":
                item.done = !item.done;
                break;
            default:
                if (typeof(item.markCurrentOccurrenceAsDone) === "function") {
                    item.markCurrentOccurrenceAsDone();
                } else {
                    console.warn("Cannot toggle done state for", item, "of type", item.itemType);
                }
            }
            swipeDelegate.toggleDoneOnClose = false;
        }
    }

    QtObject {
        id: d

        property OTL.Item parentItem: null

        function loadParentItem() {
            if (swipeDelegate.showParentItemInformation) {
                switch (swipeDelegate.item.itemType) {
                case "Todo":
                    OTL.Application.loadItem(swipeDelegate.item.todoListUid);
                    break;
                default:
                    break;
                }
            } else {
                parentItem = null;
            }
        }
    }

    Connections {
        target: OTL.Application

        function onItemLoaded(uid, data) {
            switch (swipeDelegate.item.itemType) {
            case "Todo":
                if (uid === swipeDelegate.item.todoListUid) {
                    d.parentItem = OTL.Application.itemFromData(data);
                }
                break;
            default:
                break;
            }
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
        dragTile: swipeDelegate.dragTile
        item: swipeDelegate.item
    }

    Timer {
        id: delayedClickTimer

        property bool afterDoubleClick: false

        interval: 300
        repeat: false
        onTriggered: {
            swipeDelegate.setSwipeDelegate(null);
            swipeDelegate.itemClicked();
        }
    }

    Rectangle {
        visible: swipeDelegate.drawSeperator
        width: parent.width * 0.8
        height: 2
        anchors.bottom: parent.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        color: Material.color(Material.Grey)
        opacity: 0.1
    }

    FontMetrics { id: fontMetrics }

    Actions.RenameItem { id: renameAction; item: swipeDelegate.item }
    Actions.SetDueTo { id: setDueToAction; item: swipeDelegate.item }
    Actions.DeleteItem { id: deleteAction; item: swipeDelegate.item }
    Actions.SetDueToday { id: setDueTodayAction; item: swipeDelegate.item; hideButton: true }
    Actions.SetDueTomorrow { id: setDueTomorrowAction; item: swipeDelegate.item; hideButton: true }
    Actions.SetDueThisWeek { id: setDueThisWeekAction; item: swipeDelegate.item; hideButton: true }
    Actions.SetDueNextWeek { id: setDueNextWeekAction; item: swipeDelegate.item; hideButton: true }
}
