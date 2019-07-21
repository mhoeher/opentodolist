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

    signal headerButtonClicked()
    signal todoClicked(var todo)

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

    QtObject {
        id: d

        property SwipeDelegate openSwipeDelegate: null
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
                        anchors.verticalCenter: parent.verticalCenter
                        onClicked: object.done = !object.done
                    }
                    Column {
                        Layout.fillWidth: true
                        anchors.verticalCenter: parent.verticalCenter

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
                    d.openSwipeDelegate = null;
                    reorderOverlay.startDrag();
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
