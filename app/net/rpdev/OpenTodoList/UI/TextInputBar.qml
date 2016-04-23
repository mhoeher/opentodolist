import QtQuick 2.0
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.4

import net.rpdev.OpenTodoList.UI 1.0

Item {
    id: root
    
    property TextField edit: edit
    property alias placeholderText: edit.placeholderText
    property bool showWhenNonEmpty: false
    property alias symbol: button.symbol
    property alias color: background.color
    property bool itemCreator: true
    property var createItem: itemCreator ? __createItem : null
    property var createItemAndOpen: itemCreator ? __createItemAndOpen : null
    property alias contentHeight: background.height
    property bool shown: __visible
    property bool closeOnButtonClick: false
    property alias text: edit.displayText
    
    property bool __visible: edit.focus || (showWhenNonEmpty && (edit.text !== ""))
    
    signal accepted(bool openItem)
    
    function __createItem() {
        if (edit.displayText !== "") {
            root.accepted(false);
        }
    }
    
    function __createItemAndOpen() {
        if (edit.displayText !== "") {
            root.accepted(true);
        }
    }
    
    function __createIfValidInput() {
        if (itemCreator && (edit.displayText !== "")) {
            __createItem();
        }
    }
    
    width: parent.width
    height: Math.max(edit.height, button.height) + Globals.defaultMargin * 3
    y: __visible ? 0 : -height
    z: 1
    
    Behavior on y { SmoothedAnimation { duration: 500 } }
    
    Rectangle {
        id: background
        anchors.fill: parent
        anchors.bottomMargin: Globals.defaultMargin
        color: Colors.headerBar
        
        RowLayout {
            anchors {
                left: parent.left
                right: parent.right
                margins: Globals.defaultMargin
                verticalCenter: parent.verticalCenter
            }
            height: childrenRect.height
            
            TextField {
                id: edit
                Layout.fillWidth: true
                Keys.onEscapePressed: focus = false
                Keys.onBackPressed: focus = false
                Keys.onEnterPressed: __createIfValidInput()
                Keys.onReturnPressed: __createIfValidInput()
            }
            
            Symbol {
                id: button
                
                symbol: Fonts.symbols.faPlus
                enabled: (edit.text !== "") || root.closeOnButtonClick
                onClicked: {
                    if (root.closeOnButtonClick) {
                        edit.text = "";
                    }
                    button.forceActiveFocus();
                    __createIfValidInput()
                }
            }
        }
    }
    
    Rectangle {
        anchors {
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }
        height: Globals.defaultMargin
        gradient: Gradient {
            GradientStop {
                position: 0
                color: "#88000000"
            }
            GradientStop {
                position: 1
                color: "transparent"
            }
        }
    }
}
