import QtQuick 2.10
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.0

import OpenTodoList 1.0 as OTL

import "../Components" as Components
import "../Utils"
import "../Fonts"
import "../Windows"

ListView {
    id: root

    property Component headerComponent: null
    property Component footerComponent: null

    property Components.ToolButton headerIcon: headerItem ? headerItem.headerIcon : null
    property Components.ToolButton headerIcon2: headerItem ? headerItem.headerIcon2 : null

    property string symbol: Icons.faPlus
    property string symbolFont: Fonts.fontAwesomeSolid.name
    property bool headerItemVisible: true

    property string symbol2
    property string symbolFont2: Fonts.fontAwesomeSolid.name
    property bool headerItem2Visible: false

    property string title
    property bool allowCreatingNewItems: false
    property string newItemPlaceholderText
    property bool allowSorting: true
    property bool allowSettingDueDate: false

    signal headerButtonClicked()
    signal headerButton2Clicked()
    signal todoClicked(var todo)
    signal createNewItem(string title, var args)

    header: Pane {
        property alias headerIcon: headerIcon
        property alias headerIcon2: headerIcon2

        width: parent.width
        height: column.contentHeight

        Column {
            id: column

            width: parent.width

            Loader {
                width: parent.width
                sourceComponent: root.headerComponent
            }

            RowLayout {
                width: parent.width

                Components.Heading {
                    id: headerLabel

                    level: 2
                    Layout.fillWidth: true
                    font.bold: true
                    text: root.title
                }

                Item {
                    visible: !headerIcon2.visible
                    width: headerIcon2.width
                    height: headerIcon2.height
                }

                Components.ToolButton {
                    id: headerIcon2

                    symbol: root.symbol2
                    font.family: root.symbolFont2
                    onClicked: root.headerButton2Clicked()
                    visible: root.headerItem2Visible
                }

                Item {
                    visible: !headerIcon.visible
                    width: headerIcon.width
                    height: headerIcon.height
                }

                Components.ToolButton {
                    id: headerIcon

                    symbol: root.symbol
                    font.family: root.symbolFont
                    onClicked: root.headerButtonClicked()
                    visible: root.headerItemVisible
                }
            }

            DateSelectionDialog {
                id: dateDialog
                onAccepted: newItemRow.dueDate = selectedDate
            }

            GridLayout {
                id: newItemRow

                property var dueDate: new Date("")

                columns: 4
                visible: root.allowCreatingNewItems
                width: parent.width

                Item {
                    width: newItemButton.width
                    height: newItemButton.height
                    Layout.row: 0
                    Layout.column: 0
                }

                Components.TextField {
                    id: newItemTitelEdit
                    Layout.fillWidth: true
                    selectByMouse: true
                    Layout.row: 0
                    Layout.column: 1
                    onAccepted: newItemButton.clicked()
                    placeholderText: root.newItemPlaceholderText
                }

                Components.ToolButton {
                    symbol: Icons.faCalendarAlt
                    visible: root.allowSettingDueDate
                    Layout.row: 0
                    Layout.column: 2
                    onClicked: {
                        dateDialog.selectedDate = newItemRow.dueDate;
                        dateDialog.open();
                    }
                }

                Components.ToolButton {
                    id: newItemButton
                    symbol: Icons.faPlus
                    enabled: newItemTitelEdit.displayText !== ""
                    Layout.column: 3
                    Layout.row: 0
                    onClicked: {
                        var title = newItemTitelEdit.displayText;
                        if (title !== "") {
                            var args = {};
                            if (DateUtils.validDate(newItemRow.dueDate)) {
                                args.dueTo = newItemRow.dueDate;
                                newItemRow.dueDate = new Date("");
                            }
                            root.createNewItem(newItemTitelEdit.displayText, args);
                            newItemTitelEdit.clear();
                            newItemTitelEdit.forceActiveFocus();
                        }
                    }
                }
            }

            Label {
                text: {
                    if (DateUtils.validDate(newItemRow.dueDate)) {
                        return qsTr("Due on: %1").arg(
                                    newItemRow.dueDate.toLocaleDateString());
                    } else {
                        return "";
                    }
                }
                visible: DateUtils.validDate(newItemRow.dueDate)
                Layout.row: 1
                Layout.column: 1
                Layout.columnSpan: 2
                Layout.fillWidth: true
            }

            Components.ToolButton {
                symbol: Icons.faTimes
                onClicked: newItemRow.dueDate = new Date("")
                visible: DateUtils.validDate(newItemRow.dueDate)
                Layout.row: 1
                Layout.column: 3
            }
        }
    }

    footer: Pane {
        width: parent.width
        height: column.contentHeight

        Column {
            id: column
            width: parent.width

            Loader {
                width: parent.width
                sourceComponent: root.footerComponent
            }
        }
    }

    delegate: SwipeDelegate {
        id: swipeDelegate

        property bool toggleDoneOnClose: false

        width: parent.width
        padding: 0
        topPadding: 0
        bottomPadding: 0
        hoverEnabled: true
        contentItem: RowLayout {
            width: parent.width

            Components.ToolButton {
                font.family: Fonts.icons
                symbol: object.done ? Icons.faCheckCircle :
                                      Icons.faCircle
                onClicked: object.done = !object.done
            }
            Column {
                Layout.fillWidth: true

                Components.MarkdownLabel {
                    markdown: object.title
                    width: parent.width
                }
                Item {
                    height: 10
                    width: 1
                    visible: object.dueTo !== undefined &&
                             DateUtils.validDate(object.dueTo)
                }
                RowLayout {
                    width: parent.width
                    visible: object.dueTo !== undefined &&
                             DateUtils.validDate(object.dueTo)
                    opacity: 0.5

                    Label {
                        font.family: Fonts.icons
                        text: Icons.faCalendarAlt
                    }
                    Label {
                        Layout.fillWidth: true
                        text: DateUtils.format(object.dueTo)
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

            Components.ToolButton {
                symbol: Icons.faPencilAlt
                onClicked: {
                    renameDialog.renameItem(object);
                    swipeDelegate.swipe.close();
                }
            }
            Components.ToolButton {
                symbol: Icons.faCalendarAlt
                visible: root.allowSettingDueDate
                onClicked: {
                    dateDialogForExistingItem.item = object;
                    dateDialogForExistingItem.open();
                    swipeDelegate.swipe.close();
                }
            }
            Components.ToolButton {
                symbol: Icons.faTrash
                onClicked: deleteDialog.deleteItem(object)
            }
        }
        swipe.left: Pane {
            height: swipeDelegate.height
            width: swipeDelegate.width / 2
            Material.background: Colors.positiveColor

            Components.Label {
                text: object.done ?
                          qsTr("Swipe to mark undone") :
                          qsTr("Swipe to mark done")
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
            renameDialog.renameItem(object);
        }


        Timer {
            id: delayedClickTimer

            property bool afterDoubleClick: false

            interval: 300
            repeat: false
            onTriggered: {
                d.openSwipeDelegate = null;
                root.todoClicked(object);
            }
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
            if (root.allowSorting) {
                d.openSwipeDelegate = null;
                reorderOverlay.startDrag();
            }
        }
        swipe.onClosed: {
            if (swipeDelegate.toggleDoneOnClose) {
                object.done = !object.done;
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
            item: object
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
            model: root.model
        }
    }

    QtObject {
        id: d

        property SwipeDelegate openSwipeDelegate: null
    }

    RenameItemDialog { id: renameDialog }

    DeleteItemDialog { id: deleteDialog }

    DateSelectionDialog {
        id: dateDialogForExistingItem

        property var item: null

        onAccepted: if (item !== null) {
                        item.dueTo = selectedDate;
                    }
    }
}

