import QtQuick 2.10
import QtQuick.Layouts 1.1

import OpenTodoList 1.0 as OTL

import "../Components"
import "../Utils"
import "../Fonts"
import "../Windows"

Column {
    id: root

    property alias model: repeater.model
    property alias symbol: headerIcon.symbol
    property alias symbolFont: headerIcon.font.family
    property alias title: headerLabel.text
    property alias headerItemVisible: headerIcon.visible
    property alias allowCreatingNewItems: newItemRow.visible
    property alias newItemPlaceholderText: newItemTitelEdit.placeholderText
    property ToolButton headerItem: headerIcon
    property bool allowSorting: true
    property bool allowSettingDueDate: false

    signal headerButtonClicked()
    signal todoClicked(var todo)
    signal createNewItem(string title, var args)

    RowLayout {
        width: parent.width

        Heading {
            id: headerLabel

            level: 2
            Layout.fillWidth: true
            font.bold: true
        }

        Item {
            visible: !headerIcon.visible
            width: headerIcon.width
            height: headerIcon.height
        }

        ToolButton {
            id: headerIcon

            symbol: Icons.faPlus
            onClicked: root.headerButtonClicked()
        }
    }

    RenameItemDialog { id: renameDialog }

    DeleteItemDialog { id: deleteDialog }

    DateSelectionDialog {
        id: dateDialog
        onAccepted: newItemRow.dueDate = selectedDate
    }

    QtObject {
        id: d

        property SwipeDelegate openSwipeDelegate: null
    }

    GridLayout {
        id: newItemRow

        property var dueDate: new Date("")

        columns: 4
        visible: false
        width: parent.width

        Item {
            width: newItemButton.width
            height: newItemButton.height
            Layout.row: 0
            Layout.column: 0
        }

        TextField {
            id: newItemTitelEdit
            Layout.fillWidth: true
            selectByMouse: true
            Layout.row: 0
            Layout.column: 1
            onAccepted: newItemButton.clicked()
        }

        ToolButton {
            symbol: Icons.faCalendarAlt
            visible: root.allowSettingDueDate
            Layout.row: 0
            Layout.column: 2
            onClicked: {
                dateDialog.selectedDate = newItemRow.dueDate;
                dateDialog.open();
            }
        }

        ToolButton {
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

        ToolButton {
            symbol: Icons.faTimes
            onClicked: newItemRow.dueDate = new Date("")
            visible: DateUtils.validDate(newItemRow.dueDate)
            Layout.row: 1
            Layout.column: 3
        }
    }

    Column {
        width: parent.width
        clip: true
        spacing: 10

        Repeater {
            id: repeater

            delegate: SwipeDelegate {
                id: swipeDelegate

                width: parent.width
                padding: 0
                topPadding: 0
                bottomPadding: 0
                hoverEnabled: true
                contentItem: RowLayout {
                    width: parent.width

                    ToolButton {
                        font.family: Fonts.icons
                        symbol: object.done ? Icons.faCheckCircle :
                                              Icons.faCircle
                        onClicked: object.done = !object.done
                    }
                    Column {
                        Layout.fillWidth: true

                        MarkdownLabel {
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

                    ToolButton {
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

                    ToolButton {
                        symbol: Icons.faPencilAlt
                        onClicked: renameDialog.renameItem(object)
                    }
                    ToolButton {
                        symbol: Icons.faTrash
                        onClicked: deleteDialog.deleteItem(object)
                    }
                }
                onClicked: {
                    d.openSwipeDelegate = null;
                    root.todoClicked(object);
                }
                onPressAndHold: {
                    if (root.allowSorting) {
                        d.openSwipeDelegate = null;
                        reorderOverlay.startDrag();
                    }
                }
                swipe.onOpened: d.openSwipeDelegate = swipeDelegate

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
        }
    }
}
