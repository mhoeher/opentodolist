pragma Singleton

import QtQuick 2.0

import OpenTodoList 1.0 as OTL

Item {
    id: root

    property OTL.SyntaxHighlighter syntaxHighlighter: OTL.SyntaxHighlighter {
        theme: Colors.usingDarkColorTheme ? OTL.SyntaxHighlighter.Dark : OTL.SyntaxHighlighter.Light
    }

    readonly property string stylesheet: "<style type='text/css' rel='stylesheet'>"
                                         + stylesheetContent + "</style>"

    readonly property string stylesheetContent: "
a, a:visited {
color: %1;
text-decoration: none;
}
h6 {
font-style: italic;
}

code {
color: %2;
white-space: pre-wrap;
}

pre {
white-space: pre-wrap;
}

table {
margin-top: 0.5em;
margin-bottom: 0.5em;
}
td, th {
padding-left: 0.4em;
padding-right: 0.4em;
}
".arg(Colors.linkColor).arg(Colors.color(Colors.negativeColor))

    function markdownToHtml(text) {
        if (text === "" || !text) {
            return ""
        }
        return textUtils.markdownToHtml(text, stylesheetContent,
                                        syntaxHighlighter)
    }

    function markdownToPlainText(text) {
        return textUtils.htmlToPlainText(textUtils.markdownToHtml(text))
    }


    /*
      Returns the first non-empty plain text line from the markdown text.
     */
    function firstPlainTextLineFromMarkdown(markdown) {
        let plain = markdownToPlainText(markdown)
        let lines = plain.split(/\r?\n/)
        for (var i = 0; i < lines.length; ++i) {
            let line = lines[i]
            if (line.length > 0) {
                return line
            }
        }
        return ""
    }

    OTL.TextUtils {
        id: textUtils
    }
}
