Qt.include("showdownjs/showdown.js")

var converter = new showdown.Converter();
converter.setFlavor('github');
// Note: Seens not to be supported by QML/Qt :(
converter.setOption('strikethrough', true);
converter.setOption('ghMentions', false);

function format(markdown) {
    var result = converter.makeHtml(markdown);
    var count = (result.match(/<p>/g) || []).length;
    if (count === 1 && result.startsWith("<p>") &&
            result.endsWith("</p>")) {
        result = result.substring(3, result.length - 4);
    }
    return result;
}


WorkerScript.onMessage = function(msg) {
    var text = msg.text;
    WorkerScript.sendMessage({ text: text, result: format(text) });
};
