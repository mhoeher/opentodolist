import QtQuick 2.0
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.1
import QtGraphicalEffects 1.0 as GE

import net.rpdev.OpenTodoList 1.0
import net.rpdev.OpenTodoList.UI 1.0

MouseArea {
    id: item
    
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
                        color: Colors.itemDelimiter
                        antialiasing: true
                    }
                }
            }
        }
        
        Label {
            id: title
            
            text: libraryItem.title
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
                model: FilterModel {
                    id: todosModel
                    
                    sourceModel: TodosModel {
                        todoList: item.libraryItem
                    }
                    filterFunction: function(row) {
                        var i = sourceModel.index(row, 0);
                        var todo = sourceModel.data(i, TodosModel.TodoRole);
                        return !todo.done;
                    }
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
                            text: todo.title
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


