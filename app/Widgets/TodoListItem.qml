import QtQuick 2.0
import QtQuick.Layouts 1.1

import OpenTodoList 1.0 as OTL

import "../Components"
import "../Utils"
import "../Fonts"

Item {
    id: item

    property OTL.Library library: null
    property OTL.TodoList libraryItem: OTL.TodoList {}

    signal clicked()
    signal released(var mouse)

    Pane {
        anchors.fill: parent
        anchors.margins: 5
        elevation: 6
        padding: 0

        Pane {
            id: title

            anchors {
                left: parent.left
                right: parent.right
                top: parent.top
            }
            height: titleLabel.height + padding
            backgroundColor: Colors.color(Colors.itemColor(item.libraryItem),
                                          Colors.shade100)

            MarkdownLabel {
                id: titleLabel
                markdown: item.libraryItem.title
                wrapMode: Text.NoWrap
                elide: Text.ElideRight
                width: parent.width
            }
        }

        Pane {
            anchors {
                left: parent.left
                right: parent.right
                top: title.bottom
                bottom: parent.bottom
            }
            backgroundColor: Colors.color(Colors.itemColor(item.libraryItem),
                                          Colors.shade50)

            ListView {
                id: openTodosList

                width: parent.width
                height: parent.height
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
        onClicked: item.clicked()
        onReleased: item.released(mouse)
    }
}
