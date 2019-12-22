import QtQuick 2.0
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12

import OpenTodoList 1.0 as OTL

import "../Components"
import "../Utils"
import "../Fonts"

Item {
    id: item

    property OTL.Library library: null
    property OTL.TodoList libraryItem: OTL.TodoList {}
    property var model
    property bool allowReordering: true

    signal clicked()
    signal released(var mouse)

    ItemPane {
        anchors.fill: parent
        anchors.margins: 5
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

            MarkdownLabel {
                id: titleLabel
                markdown: item.libraryItem.title
                wrapMode: Text.NoWrap
                elide: Text.ElideRight
                width: parent.width
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

            ListView {
                id: openTodosList

                width: parent.width
                height: parent.height
                spacing: 5
                interactive: false
                clip: true
                model: OTL.ItemsSortFilterModel {
                    sourceModel: OTL.ItemsModel {
                        cache: OTL.Application.cache
                        parentItem: item.libraryItem.uid
                        onlyUndone: true
                    }
                }
                delegate: RowLayout {
                    width: parent.width
                    Label {
                        font.family: Fonts.icons
                        text: Icons.faCircle
                    }
                    MarkdownLabel {
                        markdown: object.title
                        elide: Text.ElideRight
                        Layout.fillWidth: true
                        wrapMode: Text.NoWrap
                    }
                }
            }

            Label {
                width: parent.width
                height: parent.height
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
                text: qsTr("✔ No open todos - everything done")
                visible: openTodosList.count === 0
                opacity: 0.5
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
    }
}
