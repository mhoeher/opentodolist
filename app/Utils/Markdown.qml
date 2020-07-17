pragma Singleton

import QtQuick 2.0

import OpenTodoList 1.0 as OTL

import "./TextUtils.js" as TextUtils

Item {
    id: root

    property OTL.SyntaxHighlighter syntaxHighlighter: OTL.SyntaxHighlighter {
        theme: Colors.usingDarkColorTheme ? OTL.SyntaxHighlighter.Dark : OTL.SyntaxHighlighter.Light
    }

    readonly property string stylesheet: "
    <style type='text/css'>
        a, a:visited {
            color: %1;
            text-decoration: none;
        }
        h6 {
            font-style: italic;
        }

        code {
            color: %2;
        }

        del {
            text-decoration: line-through;
        }

        table {
            margin-top: 0.5em;
            margin-bottom: 0.5em;
        }
        td, th {
            padding-left: 0.4em;
            padding-right: 0.4em;
        }
    </style>
    ".arg(Colors.linkColor).arg(Colors.color(Colors.negativeColor))

    function markdownToHtml(text) {
        return stylesheet + TextUtils.markdownToHtml(text);
    }

    function markdownToPlainText(text) {
        return OTL.Application.htmlToPlainText(TextUtils.markdownToHtml(text));
    }
}
