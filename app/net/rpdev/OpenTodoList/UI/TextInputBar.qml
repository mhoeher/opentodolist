import QtQuick 2.0
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.4

import net.rpdev.OpenTodoList.UI 1.0

Item {
    id: root
    
    property TextField edit: edit
    property alias placeholderText: edit.placeholderText
    
    signal accepted(bool openItem)
    
    function createItem() {
        if (edit.displayText !== "") {
            root.accepted(false);
        }
    }
    
    function createItemAndOpen() {
        if (edit.displayText !== "") {
            root.accepted(true);
        }
    }
    
    width: parent.width
    height: Math.max(edit.height, button.height) + Globals.defaultMargin * 3
    y: edit.focus ? 0 : -height
    z: 1
    
    Behavior on y { SmoothedAnimation { duration: 500 } }
    
    Rectangle {
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
                Keys.onEnterPressed: if (edit.text !== "") root.createItem()
                Keys.onReturnPressed: if (edit.text !== "") root.createItem()
            }
            
            Symbol {
                id: button
                
                symbol: Fonts.symbols.faPlus
                enabled: edit.text !== ""
                onClicked: {
                    edit.focus = false;
                    root.createItem()
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
