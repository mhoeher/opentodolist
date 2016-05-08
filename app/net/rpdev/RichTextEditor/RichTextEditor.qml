import QtQuick 2.0
import QtWebView 1.1

Item {
    id: editor
    
    readonly property bool isRichTextEditor: true
    property string text: qsTr("<b>Rich</b> <i>Text</i> <u>Editor</u>")
    
    function toggleBold() {
        view.runJavaScript("RichTextEditor.bold();");
    }
    
    function toggleItalic() {
        view.runJavaScript("RichTextEditor.italic();");
    }
    
    function toggleUnderline() {
        view.runJavaScript("RichTextEditor.underline();");
    }
    
    function toggleStrikeThrough() {
        view.runJavaScript("RichTextEditor.strikeThrough();");
    }
    
    function copy() {
        view.triggerWebAction(WebEngineView.Copy);
    }
    
    function cut() {
        view.triggerWebAction(WebEngineView.Cut);
    }
    
    function paste() {
        view.triggerWebAction(WebEngineView.Paste);
    }
    
    function indent() {
        view.runJavaScript("RichTextEditor.indent();");
    }
    
    function outdent() {
        view.runJavaScript("RichTextEditor.outdent();");
    }
    
    function alignLeft() {
        view.runJavaScript("RichTextEditor.alignLeft();");
    }
    
    function alignCenter() {
        view.runJavaScript("RichTextEditor.alignCenter();");
    }
    
    function alignRight() {
        view.runJavaScript("RichTextEditor.alignRight();");
    }
    
    function alignJustify() {
        view.runJavaScript("RichTextEditor.alignJustify();");
    }
    
    function insertUnorderedList() {
        view.runJavaScript("RichTextEditor.insertUnorderedList();");
    }
    
    function insertOrderedList() {
        view.runJavaScript("RichTextEditor.insertOrderedList();");
    }
    
    function heading1() {
        view.runJavaScript("RichTextEditor.block('H1');");
    }
    
    function heading2() {
        view.runJavaScript("RichTextEditor.block('H2');");
    }
    
    function heading3() {
        view.runJavaScript("RichTextEditor.block('H3');");
    }
    
    function heading4() {
        view.runJavaScript("RichTextEditor.block('H4');");
    }
    
    function heading5() {
        view.runJavaScript("RichTextEditor.block('H5');");
    }
    
    function heading6() {
        view.runJavaScript("RichTextEditor.block('H6');");
    }
    
    function defaultStyle() {
        view.runJavaScript("RichTextEditor.block('P');");
    }
    
    function code() {
        view.runJavaScript("RichTextEditor.block('PRE');");
    }
    
    /**
      @brief Finish editing the document - call this before reading back the text property!
      */
    function finishEditing() {
        view.loadText();
    }
    
    onTextChanged: view.saveText()
    
    WebView {
        id: view
        
        function saveText() {
            if (!loading) {
                view.runJavaScript("RichTextEditor.setText(%1);".arg(JSON.stringify(text)));
            }
        }
        
        function loadText() {
            view.runJavaScript("RichTextEditor.getText()", function(currentText) {
                if (currentText !== editor.text) {
                    editor.text = currentText;
                }
            });
        }
        
        anchors.fill: parent
        url: Qt.resolvedUrl("index.html")
        
        onLoadingChanged: {
            if (!loading) {
                saveText();
            }
        }
        
        onTitleChanged: {
            if (title === "modified") {
                loadText();
            }
        }
    }
}
