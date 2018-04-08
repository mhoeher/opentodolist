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
    property alias decorativeSymbol: decorativeIcon.symbol
    property alias decorativeSymbolFont: decorativeIcon.font.family
    property alias symbol: headerIcon.symbol
    property alias symbolFont: headerIcon.font.family
    property alias title: headerLabel.text

    signal headerButtonClicked()
    signal todoClicked(var todo)

    RowLayout {
        width: parent.width

        ToolButton {
            id: decorativeIcon
            symbol: Icons.faTags
            background: Item {}
        }

        Label {
            id: headerLabel

            Layout.fillWidth: true
            font.bold: true
        }

        ToolButton {
            id: headerIcon

            symbol: Icons.faPlus
            onClicked: root.headerButtonClicked()
        }
    }

    RenameItemDialog { id: renameDialog }

    DeleteItemDialog { id: deleteDialog }

    Column {
        x: decorativeIcon.width
        width: parent.width - x
        clip: true

        Repeater {
            id: repeater

            delegate: ReorderableListViewOverlay {
                model: root.model
                width: parent.width
                height: swipeDelegate.height

                SwipeDelegate {
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
                            background: Item {}
                        }
                        Label {
                            Layout.fillWidth: true
                            anchors.verticalCenter: parent.verticalCenter
                            text: Markdown.format(object.title)
                        }
                        ToolButton {
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
                    onClicked: root.todoClicked(object)
                }
            }
        }
    }
}
