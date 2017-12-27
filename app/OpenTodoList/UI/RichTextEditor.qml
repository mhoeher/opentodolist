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
        textArea.forceActiveFocus();
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

    Flickable {
        id: flickable
        anchors.fill: parent
        flickableDirection: Flickable.VerticalFlick
        ScrollBar.vertical: ScrollBar {}
        TextArea.flickable: TextArea {
            id: textArea

            padding: Globals.defaultMargin
            anchors.fill: parent
            textFormat: TextEdit.PlainText
            focus: true
            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
            selectByMouse: true
            persistentSelection: true
            font.family: "Courier"
            background: Rectangle {
                id: editorBackground

                color: Colors.window
            }
        }
    }
}
