var RichTextEditor = RichTextEditor || {};


(function(RTE) {
    
    document.title = "";
    
    function load() {
        window.document.designMode = "On";
        window.document.getElementsByTagName("BODY")[0].focus();
    }
    
    function applyText() {
        document.title = "modified";
    }

    function execCommand(command, defaultUI, arg) {
        window.document.execCommand(command, defaultUI, arg);
        applyText();
    }
    
    function bold() {
        execCommand("bold");
    }
    
    function italic() {
        execCommand("italic");
    }
    
    function underline() {
        execCommand("underline");
    }
    
    function strikeThrough() {
        execCommand("strikeThrough");
    }
    
    function indent() {
        execCommand("indent");
    }
    
    function outdent() {
        execCommand("outdent");
    }
    
    function alignLeft() {
        execCommand("justifyLeft");
    }
    
    function alignCenter() {
        execCommand("justifyCenter");
    }
    
    function alignRight() {
        execCommand("justifyRight");
    }
    
    function alignJustify() {
        execCommand("justifyFull");
    }
    
    function insertUnorderedList() {
        execCommand("insertUnorderedList");
    }
    
    function insertOrderedList() {
        execCommand("insertOrderedList");
    }
    
    function block(elem) {
        execCommand("formatBlock", true, elem);
    }
    
    function setText(text) {
        var body = window.document.getElementsByTagName("BODY")[0];
        if (body.innerHTML !== text) {
            body.innerHTML = text;
        }
    }
    
    function getText() {
        var result = window.document.getElementsByTagName("BODY")[0].innerHTML;
        document.title = "";
        return result;
    }
    
    RTE.load = load;
    RTE.bold = bold;
    RTE.italic = italic;
    RTE.underline = underline;
    RTE.strikeThrough = strikeThrough;
    RTE.indent = indent;
    RTE.outdent = outdent;
    RTE.alignLeft = alignLeft;
    RTE.alignCenter = alignCenter;
    RTE.alignRight = alignRight;
    RTE.alignJustify = alignJustify;
    RTE.insertUnorderedList = insertUnorderedList;
    RTE.insertOrderedList = insertOrderedList;
    RTE.block = block;
    RTE.textChanged = applyText;
    RTE.setText = setText;
    RTE.getText = getText;
    
    // Disable context menu:
    window.document.addEventListener("contextmenu", function(event) {
        event.preventDefault();
    }, false);
    
})(RichTextEditor);


