import QtQuick 2.0
import QtQuick.Layouts 1.1
import QtQuick.Controls.Material 2.12

import OpenTodoList 1.0 as OTL

import "../Components"
import "../Controls" as C
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

            C.Label {
                id: titleLabel
                text: Markdown.markdownToPlainText(item.libraryItem.title)
                textFormat: Text.PlainText
                wrapMode: Text.NoWrap
                minimumPointSize: 6
                elide: Text.ElideRight
                width: parent.width
                fontSizeMode: Text.HorizontalFit

                C.ToolTip {
                    text: text
                    visible: titleLabel.truncated && title.hovered
                }
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

            C.Label {
                id: dueToLabel
                text: {
                    let dueTo = item.libraryItem.effectiveDueTo;
                    if (DateUtils.validDate(dueTo)) {
                        return qsTr("Due on %1").arg(DateUtils.format(dueTo));
                    }
                    return "";
                }
                visible: text !== ""
                width: parent.width
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                opacity: 0.5
            }

            C.Label {
                anchors.fill: parent
                anchors.topMargin: {
                    if (dueToLabel.visible) {
                    dueToLabel.height + AppSettings.smallSpace;
                    } else {
                        return 0;
                    }
                }
                text: Markdown.markdownToHtml(item.libraryItem.notes)
                textFormat: Text.RichText
                wrapMode: Text.NoWrap
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
