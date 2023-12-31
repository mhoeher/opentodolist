import QtQuick 2.10
import QtQuick.Layouts 1.1
import QtQuick.Controls.Material 2.0

import OpenTodoList 1.0 as OTL

import "../Components" as Components
import "../Components/Tooltips" as Tooltips
import "../Controls" as C
import "../Utils"
import "../Fonts"
import "../Windows"
import "../Actions" as Actions

C.SwipeDelegate {
    id: swipeDelegate

    property bool showParentItemInformation: false
    property var model: null
    property OTL.LibraryItem item: OTL.LibraryItem {}
    property OTL.LibraryItem parentItem: OTL.LibraryItem {}
    property OTL.Library library: null
    property bool hideDueDate: false
    readonly property var itemActions: ([renameAction, copyTodoAction, moveTodoAction, promoteTaskAction, setDueTodayAction, setDueTomorrowAction, setDueNextWeekAction, setDueThisWeekAction, setDueToAction, resetDueToAction, deleteAction])
    property bool allowReordering: false
    property bool drawSeperator: true
    property alias leftColorSwatch: leftColorSwatch

    signal itemPressedAndHold
    signal itemClicked

    width: parent ? parent.width : implicitWidth
    padding: 0
    topPadding: AppSettings.effectiveFontMetrics.height / (AppSettings.useCompactTodoLists ? 8 : 2)
    bottomPadding: topPadding
    hoverEnabled: true
    ListView.delayRemove: moveButton.dragTile.dragging
    opacity: item?.isFutureInstance ? 0.5 : 1.0

    contentItem: RowLayout {
        width: parent.width

        C.ToolButton {
            id: leftToolButton

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
                if (swipeDelegate.item.isFutureInstance) {
                    // If this is a future instance, ask the user if we really wants to mark it
                    // as done:
                    markFutureInstanceAsDone.item = swipeDelegate.item
                    markFutureInstanceAsDone.visible = true
                    return
                }

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

        ItemDragButton {
            id: moveButton
            item: swipeDelegate.item
            model: swipeDelegate.library
            listViewItem: swipeDelegate
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
            symbol: Icons.mdiMoreVert
            onClicked: swipeDelegate.itemPressedAndHold()
        }
    }
    swipe.right: Row {
        anchors.right: parent.right
        height: parent.height

        C.Label {
            text: qsTr("More Actions...")
            anchors.verticalCenter: parent.verticalCenter
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
        if (swipe.position < 0) {
            // Swipe from right to left - show context menu
            swipeDelegate.itemPressedAndHold()
        }
        swipe.close()
    }

    onPressAndHold: swipeDelegate.itemPressedAndHold()

    Rectangle {
        id: leftColorSwatch

        height: parent.height
        width: AppSettings.smallSpace
        color: "transparent"
        visible: false
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

        function onItemLoaded(uid, dataparents, library, transactionId) {
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
        dragTile: moveButton.dragTile
        item: swipeDelegate.item
    }

    Timer {
        id: delayedClickTimer

        property bool afterDoubleClick: false

        interval: 300
        repeat: false
        onTriggered: swipeDelegate.itemClicked()
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
        itemUtils: swipeDelegate.C.ApplicationWindow.window.itemUtils
    }
    Actions.SetDueTo {
        id: setDueToAction
        item: swipeDelegate.item
        itemUtils: swipeDelegate.C.ApplicationWindow.window.itemUtils
    }
    Actions.DeleteItem {
        id: deleteAction
        item: swipeDelegate.item
        itemUtils: swipeDelegate.C.ApplicationWindow.window.itemUtils
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
        itemUtils: swipeDelegate.C.ApplicationWindow.window.itemUtils
    }
    Actions.CopyTodo {
        id: copyTodoAction
        item: swipeDelegate.item
        enabled: item.itemType === "Todo"
        itemUtils: swipeDelegate.C.ApplicationWindow.window.itemUtils
    }
    Actions.PromoteTask {
        id: promoteTaskAction
        item: swipeDelegate.item
        library: swipeDelegate.library
        enabled: item.itemType === "Task"
        todoList: swipeDelegate.parentItem
        itemUtils: swipeDelegate.C.ApplicationWindow.window.itemUtils
    }

    Tooltips.MarkFutureInstanceAsDone {
        id: markFutureInstanceAsDone
    }
}
