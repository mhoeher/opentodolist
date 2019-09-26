import QtQuick 2.0
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12

import "../Components" as Components
import "../Fonts"

Pane {
    id: root
    
    property Components.TextField edit: edit
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
    
    width: parent.width
    y: __visible ? 0 : -height
    z: 1
    Material.elevation: 6
    visible: y > -height
    
    Behavior on y { SmoothedAnimation { duration: 500 } }

    RowLayout {
        anchors {
            left: parent.left
            right: parent.right
            verticalCenter: parent.verticalCenter
        }
        height: childrenRect.height

        Components.TextField {
            id: edit
            Layout.fillWidth: true
            onAccepted: __createIfValidInput()
            selectByMouse: true
            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
        }

        Components.ToolButton {
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
