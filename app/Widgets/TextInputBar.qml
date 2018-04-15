import QtQuick 2.0
import QtQuick.Layouts 1.1

import "../Components"
import "../Fonts"

Pane {
    id: root
    
    property TextField edit: edit
    property alias placeholderText: edit.placeholderText
    property bool showWhenNonEmpty: false
    property alias symbol: button.symbol
    property bool itemCreator: true
    property bool shown: __visible
    property bool closeOnButtonClick: false
    property alias text: edit.displayText
    
    property bool __visible: edit.focus || (showWhenNonEmpty && (edit.text !== ""))
    
    signal accepted()
    
    function __createIfValidInput() {
        if (edit.displayText !== "") {
            root.accepted();
        }
    }
    
    backgroundColor: info
    width: parent.width
    y: __visible ? 0 : -height
    z: 1
    elevation: 6
    visible: y > -height
    
    Behavior on y { SmoothedAnimation { duration: 500 } }

    RowLayout {
        anchors {
            left: parent.left
            right: parent.right
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
            selectByMouse: true
            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
        }

        ToolButton {
            id: button

            symbol: Icons.faPlus
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
