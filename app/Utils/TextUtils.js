.import Utils 1.0 as Utils

Qt.include("showdown-1.9.1/dist/showdown.js")

// Register custom extension which highlights code blocks using
// the KDE Syntax Highlighter.
// Sample code taken from https://github.com/showdownjs/showdown/issues/577
showdown.extension('highlight', function () {
    return [{
                type: "output",
                filter: function (text, converter, options) {
                    var left = "<pre><code\\b[^>]*>",
                    right = "</code></pre>",
                    flags = "g";
                    var replacement = function (wholeMatch, match, left, right) {
                        var lang = (left.match(/class=\"([^ \"]+)/) || [])[1];
                        return Utils.SyntaxHighlighter.sourceToHtml(match, lang);
                    };
                    return showdown.helper.replaceRecursiveRegExp(text, replacement, left, right, flags);
                }
            }];
});

// Create global converter used to convert Markdown to HTML:
//var converter = new showdown.Converter();
var converter = new showdown.Converter({ extensions: ['highlight']});
converter.setFlavor('github');
converter.setOption('strikethrough', true);
converter.setOption('ghMentions', false);
converter.setOption('disableForced4SpacesIndentedSublists', true);

function markdownToHtml(markdown) {
    if (markdown !== "") {
        var result = converter.makeHtml(markdown);
        var count = (result.match(/<p>/g) || []).length;
        if (count === 1 && result.startsWith("<p>") &&
                result.endsWith("</p>")) {
            result = result.substring(3, result.length - 4);
        }
        return result;
    } else {
        return "";
    }
}
