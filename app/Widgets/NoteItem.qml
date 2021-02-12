import QtQuick 2.0
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12

import OpenTodoList 1.0 as OTL

import "../Components"
import "../Utils"

Item {
    id: item

    property OTL.Library library: null
    property OTL.Note libraryItem: OTL.Note {}
    property var model
    property bool allowReordering: true
    property ItemDragTile dragTile

    signal clicked()
    signal released(var mouse)

    ItemPane {
        anchors.fill: parent
        anchors.margins: AppSettings.smallSpace
        Material.elevation: 6
        item: item.libraryItem
        padding: 0

        ItemPane {
            id: title

            anchors {
                left: parent.left
                right: parent.right
                top: parent.top
            }
            height: titleLabel.height + padding * 2
            item: item.libraryItem
            shade: midShade
            clip: true
            hoverEnabled: true

            Label {
                id: titleLabel
                text: Markdown.markdownToPlainText(item.libraryItem.title)
                textFormat: Text.PlainText
                wrapMode: Text.NoWrap
                minimumPointSize: 6
                elide: Text.ElideRight
                width: parent.width
                fontSizeMode: Text.HorizontalFit

                ToolTip.text: text
                ToolTip.visible: truncated && title.hovered
                ToolTip.delay: 200
                ToolTip.timeout: 30000
            }
        }

        ItemPane {
            anchors {
                left: parent.left
                right: parent.right
                top: title.bottom
                bottom: parent.bottom
            }
            item: item.libraryItem

            Label {
                anchors.fill: parent
                text: Markdown.markdownToHtml(item.libraryItem.notes)
                textFormat: Text.RichText
                elide: Text.ElideRight
                clip: true
            }
        }
    }

    MouseArea {
        anchors.fill: parent
        acceptedButtons: Qt.LeftButton | Qt.RightButton
        onClicked: {
            if (mouse.button === Qt.LeftButton) {
                item.clicked()
            }
        }
        onPressAndHold: if (item.allowReordering) {
                            reorderOverlay.startDrag();
                        }
        onReleased: item.released(mouse)
    }

    ReorderableListViewOverlay {
        id: reorderOverlay
        anchors.fill: parent
        model: item.model
        layout: Qt.Horizontal
        item: libraryItem
        dragTile: item.dragTile
    }
}
