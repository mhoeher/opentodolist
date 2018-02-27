import QtQuick 2.5
import QtQuick.Controls 2.2

import OpenTodoList 1.0
import OpenTodoList.UI 1.0

Page {
    id: editor
    
    property ComplexItem item: null

    readonly property TextArea textArea: textArea
    property alias text: textArea.text
    property alias backgroundColor: editorBackground.color
    
    signal accepted()
    
    function cancel() {
        textArea.focus = false;
        accepted();
    }

    Component.onCompleted: {
        if (item !== null) {
            text = item.notes;
        }
    }
    Component.onDestruction: {
        if (item !== null) {
            item.notes = text;
        }
    }

    Connections {
        target: item
        ignoreUnknownSignals: true
        onNotesChanged: editor.text = item.notes
    }

    Rectangle {
        id: editorBackground
        anchors.fill: parent
        color: Colors.window
    }

    Flickable {
        id: flickable
        anchors.fill: parent
        flickableDirection: Flickable.VerticalFlick
        ScrollBar.vertical: ScrollBar {}
        contentHeight: textArea.height + 2 * Globals.defaultMargin
        contentWidth: width

        MouseArea {
            width: flickable.width
            height: flickable.height
            onClicked: textArea.forceActiveFocus()
        }

        TextArea {
            id: textArea

            padding: Globals.defaultMargin
            x: Globals.defaultMargin
            y: x
            width: flickable.width - 2 * x
            textFormat: TextEdit.PlainText
            wrapMode: Text.WordWrap
            selectByMouse: {
                switch (Qt.platform.os) {
                case "android":
                    return focus;
                default:
                    return true;
                }
            }

            font.family: "Courier New, Courier, Fixed"

            Keys.onEscapePressed: focus = false
            Keys.onBackPressed: focus = false
        }
    }
}
