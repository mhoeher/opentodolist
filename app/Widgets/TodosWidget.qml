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

        Repeater {
            id: repeater

            delegate: SwipeDelegate {
                width: parent.width
                padding: 0
                topPadding: 0
                bottomPadding: 0
                contentItem: Row {
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
            }
        }
    }
}
