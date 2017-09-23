import QtQuick 2.5
import QtQuick.Controls 2.2

import OpenTodoList 1.0
import OpenTodoList.UI 1.0

FocusScope {
    id: editor
    
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
    
    Image {
        anchors.fill: parent
        source: "paper/paper.png"
        fillMode: Image.Tile
    }

    ScrollView {
        anchors.fill: parent

        TextArea {
            id: textArea

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
        }
    }
}
