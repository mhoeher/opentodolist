pragma Singleton

import QtQuick 2.0

import OpenTodoList 1.0 as OTL

import "./TextUtils.js" as TextUtils

Item {
    id: root

    readonly property string stylesheet: "
    <style type='text/css'>
        a, a:visited {
            color: %1;
            text-decoration: none;
        }
    </style>
    ".arg(Colors.linkColor)

    function markdownToHtml(text) {
        return stylesheet + TextUtils.markdownToHtml(text);
    }

    function markdownToPlainText(text) {
        return OTL.Application.htmlToPlainText(TextUtils.markdownToHtml(text));
    }
}
