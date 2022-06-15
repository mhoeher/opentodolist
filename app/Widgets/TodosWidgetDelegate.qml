import QtQuick 2.10
import QtQuick.Layouts 1.1
import QtQuick.Controls.Material 2.0

import OpenTodoList 1.0 as OTL

import "../Components" as Components
import "../Controls" as C
import "../Utils"
import "../Fonts"
import "../Windows"
import "../Actions" as Actions

C.SwipeDelegate {
    id: swipeDelegate

    property bool showParentItemInformation: false
    property var model: null
    property bool toggleDoneOnClose: false
    property OTL.LibraryItem item: OTL.LibraryItem {}
    property OTL.LibraryItem parentItem: OTL.LibraryItem {}
    property OTL.Library library: null
    property bool hideDueDate: false
    readonly property var itemActions: ([renameAction, copyTodoAction, moveTodoAction, promoteTaskAction, setDueTodayAction, setDueTomorrowAction, setDueNextWeekAction, setDueThisWeekAction, setDueToAction, resetDueToAction, deleteAction])
    property bool allowSorting: false
    property bool drawSeperator: true
    property ItemDragTile dragTile

    signal itemPressedAndHold
    signal itemClicked
    signal setSwipeDelegate(var item)

    // The item has been saved (for later undo)
    signal itemSaved(var itemData)

    width: parent ? parent.width : implicitWidth
    padding: 0
    topPadding: AppSettings.effectiveFontMetrics.height / (AppSettings.useCompactTodoLists ? 8 : 2)
    bottomPadding: topPadding
    hoverEnabled: true
    contentItem: RowLayout {
        width: parent.width

        C.ToolButton {
            font.family: Fonts.icons
            symbol: {
                switch (swipeDelegate.item.itemType) {
                case "Todo":
                case "Task":
                    return swipeDelegate.item.done ? Icons.mdiTaskAlt : Icons.mdiCircle
                case "TodoList":
                    return Icons.mdiChecklist
                case "Note":
                    return Icons.mdiDescription
                case "Image":
                    return Icons.mdiImage
                default:
                    return Icons.mdiHelpOutline
                }
            }
            onClicked: {
                let data = OTL.Application.saveItem(swipeDelegate.item)
                switch (swipeDelegate.item.itemType) {
                case "Task":
                case "Todo":
                    swipeDelegate.item.done = !swipeDelegate.item.done
                    break
                case "TodoList":
                case "Note":
                case "Image":
                    swipeDelegate.item.markCurrentOccurrenceAsDone()
                    break
                default:
                    return
                }
                swipeDelegate.itemSaved(data)
            }
        }
        Column {
            Layout.fillWidth: true
            spacing: AppSettings.smallSpace

            C.Label {
                text: Markdown.markdownToHtml(swipeDelegate.item.title)
                textFormat: Text.RichText
                width: parent.width
                font.strikeout: {
                    if (item && item.done !== undefined) {
                        if (item.done) {
                            return true
                        }
                    }
                    return false
                }
            }
            Item {
                height: AppSettings.effectiveFontMetrics.height / 4
                width: 1
                visible: itemInfoGrid.height > 0
            }
            Flow {
                id: itemInfoGrid
                width: parent.width
                opacity: 0.5

                Row {
                    visible: d.parentItem
                    spacing: AppSettings.effectiveFontMetrics.height / 2

                    C.Label {
                        font.family: Fonts.icons
                        text: Icons.mdiChecklist
                        width: contentWidth
                        height: contentHeight
                        color: {
                            let result = Colors.itemColor(d.parentItem)
                            return Colors.color(result)
                        }
                    }
                    C.Label {
                        text: d.parentItem ? d.parentItem.title : ""
                    }
                    Item {
                        width: AppSettings.effectiveFontMetrics.height
                        height: AppSettings.effectiveFontMetrics.height
                    }
                }

                Row {
                    visible: swipeDelegate.item.effectiveDueTo !== undefined
                             && DateUtils.validDate(
                                 swipeDelegate.item.effectiveDueTo)
                             && !swipeDelegate.hideDueDate
                    spacing: AppSettings.effectiveFontMetrics.height / 2

                    C.Label {
                        font.family: Fonts.icons
                        text: Icons.mdiEvent
                        width: contentWidth
                        height: contentHeight
                    }
                    C.Label {
                        Layout.fillWidth: true
                        text: DateUtils.format(
                                  swipeDelegate.item.effectiveDueTo)
                    }
                }
            }

            // Show the first line from the notes contents
            C.Label {
                width: parent.width
                opacity: 0.5
                elide: Text.ElideRight
                wrapMode: Text.NoWrap
                text: {
                    let item = swipeDelegate.item
                    if (AppSettings.showNotesExcepts
                            && item.notes !== undefined) {
                        return Markdown.firstPlainTextLineFromMarkdown(
                                    item.notes)
                    }
                    return ""
                }
                visible: text !== ""
            }
        }

        C.Label {
            font.family: Fonts.icons
            text: Icons.mdiNoteAlt
            visible: swipeDelegate.item.itemType === "Todo"
                     && swipeDelegate.item.notes !== ""
            opacity: 0.5
        }

        C.Label {
            visible: swipeDelegate.item.itemType === "Todo"
                     && swipeDelegate.item.numSubtasks > 0
            leftPadding: AppSettings.mediumSpace
            rightPadding: leftPadding
            background: Item {
                Rectangle {
                    anchors.fill: parent
                    anchors.leftMargin: AppSettings.smallSpace
                    anchors.rightMargin: AppSettings.smallSpace
                    radius: height / 2
                    color: Colors.color(Colors.positiveColor)
                }
            }
            text: {
                if (visible) {
                    let todo = swipeDelegate.item
                    return "%1/%2".arg(todo.numDoneSubtasks).arg(
                                todo.numSubtasks)
                } else {
                    return ""
                }
            }
            color: Colors.textColor(Colors.positiveColor)
            font.bold: true
        }
        Item {
            visible: !toggleSwipeOpened.visible
            width: toggleSwipeOpened.width
            height: toggleSwipeOpened.height
        }

        C.ToolButton {
            id: moveButton
            visible: {
                if (swipeDelegate.allowSorting) {
                    switch (Qt.platform.os) {
                    case "ios":
                    case "android":
                        return true
                    default:
                        return swipeDelegate.hovered
                    }
                } else {
                    return false
                }
            }
            symbol: Icons.mdiDragHandle
            font.family: Fonts.icons
            onPressed: {
                swipeDelegate.setSwipeDelegate(null)
                reorderOverlay.startDrag()
            }
        }

        C.ToolButton {
            id: toggleSwipeOpened
            visible: {
                switch (Qt.platform.os) {
                case "ios":
                case "android":
                    return false
                default:
                    return swipeDelegate.hovered
                }
            }
            symbol: swipeDelegate.swipe.position
                    === 0 ? Icons.mdiKeyboardArrowLeft : Icons.mdiKeyboardArrowRight
            onClicked: {
                if (swipeDelegate.swipe.position === 0) {
                    swipeDelegate.swipe.open(C.SwipeDelegate.Right)
                } else {
                    swipeDelegate.swipe.close()
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

    swipe.left: C.Pane {
        height: swipeDelegate.height
        width: swipeDelegate.width
        enabled: {
            let item = swipeDelegate.item
            return item.done !== undefined || item.dueTo !== undefined
        }

        Material.background: Colors.positiveColor

        C.Label {
            text: {
                let item = swipeDelegate.item
                let done = item.done
                if (done === undefined) {
                    done = !DateUtils.validDate(item.dueTo)
                }
                if (done) {
                    return qsTr("Swipe to mark undone")
                } else {
                    return qsTr("Swipe to mark done")
                }
            }
            width: parent.width / 2
            anchors.fill: parent
        }
    }

    onItemChanged: d.loadParentItem()
    onShowParentItemInformationChanged: d.loadParentItem()

    onClicked: {
        // Delay evaluation, as it overlaps with double clicks:
        if (!delayedClickTimer.afterDoubleClick) {
            delayedClickTimer.start()
        }
        delayedClickTimer.afterDoubleClick = false
    }

    onDoubleClicked: {
        // Remember that we detected a double click:
        delayedClickTimer.afterDoubleClick = true

        // Handle the double click:
        delayedClickTimer.stop()
        renameAction.trigger()
    }

    swipe.onCompleted: {
        if (swipe.position > 0) {
            // Swipe from left to right to mark items as (un)done.
            swipeDelegate.toggleDoneOnClose = true
            swipeDelegate.swipe.close()
        } else {
            swipeDelegate.setSwipeDelegate(swipeDelegate)
        }
    }

    onPressAndHold: swipeDelegate.itemPressedAndHold()
    swipe.onClosed: {
        if (swipeDelegate.toggleDoneOnClose) {
            let item = swipeDelegate.item
            switch (item.itemType) {
            case "Todo":
            case "Task":
                let data = OTL.Application.saveItem(item)
                item.done = !item.done
                swipeDelegate.itemSaved(data)
                break
            default:
                if (typeof (item.markCurrentOccurrenceAsDone) === "function") {
                    item.markCurrentOccurrenceAsDone()
                } else {
                    console.warn("Cannot toggle done state for", item,
                                 "of type", item.itemType)
                }
            }
            swipeDelegate.toggleDoneOnClose = false
        }
    }

    QtObject {
        id: d

        property OTL.LibraryItem parentItem: null

        function loadParentItem() {
            if (swipeDelegate.showParentItemInformation) {
                switch (swipeDelegate.item.itemType) {
                case "Todo":
                    OTL.Application.loadItem(swipeDelegate.item.todoListUid)
                    break
                default:
                    break
                }
            } else {
                parentItem = null
            }
        }
    }

    Connections {
        target: OTL.Application

        function onItemLoaded(uid, data) {
            switch (swipeDelegate.item.itemType) {
            case "Todo":
                if (uid === swipeDelegate.item.todoListUid) {
                    d.parentItem = OTL.Application.itemFromData(data)
                }
                break
            default:
                break
            }
        }
    }

    Connections {
        target: swipeDelegate.item
        function onTodoListUidChanged() {
            d.loadParentItem()
        }
        ignoreUnknownSignals: true
    }

    ProgressItemOverlay {
        item: swipeDelegate.item
        height: background.height
        x: {
            if (swipeDelegate.swipe.rightItem) {
                return swipeDelegate.swipe.position * swipeDelegate.swipe.rightItem.width
            } else {
                return 0
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
            swipeDelegate.setSwipeDelegate(null)
            swipeDelegate.itemClicked()
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

    Actions.RenameItem {
        id: renameAction
        item: swipeDelegate.item
    }
    Actions.SetDueTo {
        id: setDueToAction
        item: swipeDelegate.item
    }
    Actions.DeleteItem {
        id: deleteAction
        item: swipeDelegate.item
    }
    Actions.SetDueToday {
        id: setDueTodayAction
        item: swipeDelegate.item
        hideButton: true
    }
    Actions.SetDueTomorrow {
        id: setDueTomorrowAction
        item: swipeDelegate.item
        hideButton: true
    }
    Actions.SetDueThisWeek {
        id: setDueThisWeekAction
        item: swipeDelegate.item
        hideButton: true
    }
    Actions.SetDueNextWeek {
        id: setDueNextWeekAction
        item: swipeDelegate.item
        hideButton: true
    }
    Actions.ResetDueTo {
        id: resetDueToAction
        item: swipeDelegate.item
        hideButton: true
    }
    Actions.MoveTodo {
        id: moveTodoAction
        item: swipeDelegate.item
        library: swipeDelegate.library
        enabled: item.itemType === "Todo"
    }
    Actions.CopyTodo {
        id: copyTodoAction
        item: swipeDelegate.item
        enabled: item.itemType === "Todo"
    }
    Actions.PromoteTask {
        id: promoteTaskAction
        item: swipeDelegate.item
        library: swipeDelegate.library
        enabled: item.itemType === "Task"
        todoList: swipeDelegate.parentItem
    }
}
