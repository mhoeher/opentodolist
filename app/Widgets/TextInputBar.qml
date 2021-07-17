import QtQuick 2.0
import QtQuick.Layouts 1.1
import QtQuick.Controls.Material 2.12

import "../Components" as Components
import "../Controls" as C
import "../Fonts"
import "../Utils" as Utils

C.Pane {
    id: root
    
    property C.TextField edit: edit
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

        C.TextField {
            id: edit
            Layout.fillWidth: true
            onAccepted: __createIfValidInput()
            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
        }

        C.ToolButton {
            id: button

            symbol: Icons.mdiAdd
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
