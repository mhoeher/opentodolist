import QtQuick 2.5
import QtQuick.Controls 1.4

import net.rpdev.OpenTodoList 1.0
import net.rpdev.OpenTodoList.UI 1.0

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
        accepted();
    }
    
    width: 800
    height: 600
    
    Image {
        anchors.fill: parent
        source: "paper/paper.png"
        fillMode: Image.Tile
    }
    
    TextArea {
        id: textArea
        
        anchors.fill: parent
        textFormat: TextEdit.RichText
        tabChangesFocus: false
        backgroundVisible: false
    }
}
