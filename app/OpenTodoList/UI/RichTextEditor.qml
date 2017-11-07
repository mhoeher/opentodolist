import QtQuick 2.5
import QtQuick.Controls 2.2

import OpenTodoList 1.0
import OpenTodoList.UI 1.0

Page {
    id: editor
    
    property ComplexItem item: null

    readonly property TextArea textArea: textArea
    property alias text: textArea.text
    property DocumentFormatter documentFormatter: DocumentFormatter {
        target: textArea
        selectionStart: textArea.selectionStart
        selectionEnd: textArea.selectionEnd
    }
    
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
    
    Image {
        anchors.fill: parent
        source: "paper/paper.png"
        fillMode: Image.Tile
    }

    Flickable {
        id: flickable
        anchors.fill: parent
        flickableDirection: Flickable.VerticalFlick
        ScrollBar.vertical: ScrollBar {}
        TextArea.flickable: TextArea {
            id: textArea

            anchors.fill: parent
            textFormat: TextEdit.RichText
            focus: true
            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
            Keys.onTabPressed: {
                if (event.modifiers & Qt.ControlModifier) {
                    documentFormatter.decreaseIndentation();
                } else {
                    documentFormatter.increaseIndentation();
                }
                event.accepted = true;
            }
            selectByMouse: true
            persistentSelection: true
        }
    }
}
