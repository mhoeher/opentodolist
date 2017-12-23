import QtQuick 2.0
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.1
import QtGraphicalEffects 1.0 as GE

import OpenTodoList 1.0
import OpenTodoList.UI 1.0

MouseArea {
    id: item

    property Library library: null
    property TodoList libraryItem: TodoList {}

    property int __lineHeight: Globals.fontPixelSize * 1.5
    property int __linePadding: Globals.fontPixelSize * 0.25

    hoverEnabled: !Globals.touchEnabled
    acceptedButtons: Qt.LeftArrow | Qt.RightButton

    DropShadow {
        sourceItem: background
        hovered: item.containsMouse
    }

    Rectangle {
        id: background

        anchors {
            fill: parent
            margins: item.containsMouse ? Globals.defaultMargin / 2 : Globals.defaultMargin
        }
        border {
            width: 1
            color: Colors.itemDelimiter
        }
        clip: true

        Image {
            id: backgroundImage

            anchors {
                fill: parent
                margins: background.border.width
            }
            source: "paper/paper.png"
            fillMode: Image.Tile
            asynchronous: true
        }

        GE.ColorOverlay {
            source: backgroundImage
            anchors.fill: backgroundImage
            color: Colors.makeTransparent(Colors.itemColor(libraryItem.color), 0.5)
        }

        Column {
            width: parent.width

            Repeater {
                model: item.height / __lineHeight
                delegate: Item {
                    width: parent.width
                    height: __lineHeight

                    Rectangle {
                        anchors.bottom: parent.bottom
                        width: parent.width
                        height: 1
                        color: {
                            switch (item.libraryItem.color) {
                            case TopLevelItem.Lilac:
                                return Colors.itemDelimiterAlt;
                            default:
                                return Colors.itemDelimiter;
                            }
                        }
                        antialiasing: true
                    }
                }
            }
        }

        Label {
            id: title

            text: libraryItem.displayTitle
            anchors {
                left: parent.left
                leftMargin: Globals.defaultMargin
                right: parent.right
                top: parent.top
                topMargin: __lineHeight
            }
            elide: Text.ElideRight
            font {
                pixelSize: Globals.fontPixelSize
                bold: true
            }
        }

        Label {
            anchors {
                left: parent.left
                leftMargin: Globals.defaultMargin
                right: parent.right
                rightMargin: Globals.defaultMargin
                top: title.bottom
                topMargin: __lineHeight
            }
            lineHeight: __lineHeight
            lineHeightMode: Text.FixedHeight
            text: qsTr("No open todos - everything catched up :-)")
            color: "gray"
            visible: todosModel.count === 0
        }

        Column {
            anchors {
                left: parent.left
                leftMargin: Globals.defaultMargin
                right: parent.right
                rightMargin: Globals.defaultMargin
                top: title.bottom
                topMargin: __lineHeight
                bottom: parent.bottom
            }

            Repeater {
                model: ItemsSortFilterModel {
                    id: todosModel

                    sourceModel: ItemsModel {
                        container: item.library.todos
                    }

                    onlyUndone: true
                    todoList: item.libraryItem.uid
                }
                delegate: Item {
                    id: todoDelegate

                    height: Math.max(lineCheckMark.height, lineText.height)
                    anchors {
                        left: parent.left
                        right: parent.right
                    }


                    Row {
                        anchors {
                            left: parent.left
                            right: parent.right
                        }

                        Symbol {
                            id: lineCheckMark

                            y: __linePadding
                            height: __lineHeight
                            pixelSize: __lineHeight / 3 * 2
                            symbol: Fonts.symbols.faSquareO
                        }

                        Label {
                            id: lineText

                            width: todoDelegate.width - lineCheckMark.width
                            y: __linePadding
                            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                            lineHeight: __lineHeight
                            lineHeightMode: Text.FixedHeight
                            font {
                                pixelSize: Globals.fontPixelSize
                            }
                            text: object.displayTitle
                        }
                    }
                }
            }
        }

        Behavior on anchors.margins {
            SmoothedAnimation {
                duration: Globals.defaultAnimationTime
            }
        }
    }

}


