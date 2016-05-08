import QtQuick 2.5
import QtQuick.Controls 1.4

import net.rpdev.OpenTodoList 1.0
import net.rpdev.OpenTodoList.UI 1.0

import net.rpdev.RichTextEditor 1.0 as RTE

FocusScope {
    id: editor
    
    property alias text: textArea.text
    property DocumentFormatter documentFormatter: DocumentFormatter {
        target: textArea
        selectionStart: 0
        selectionEnd: 0
    }
    
    signal accepted()
    
    function cancel() {
        textArea.focus = false;
        accepted();
    }
    
    width: 800
    height: 600
    
    Image {
        anchors.fill: parent
        source: "paper/paper.png"
        fillMode: Image.Tile
    }
    
    RTE.RichTextEditor {
        id: textArea
        
        anchors.fill: parent
    }
}
