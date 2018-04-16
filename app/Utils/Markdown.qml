pragma Singleton

import QtQuick 2.0

import OpenTodoList 1.0 as OTL

import "showdownjs/showdown.js" as ShowdownJS

Item {
    readonly property string stylesheet: "
    <style type='text/css'>
        a, a:visited {
            color: %1;
            text-decoration: none;
        }
    </style>
    ".arg(Colors.color(Colors.linkColor))

    function format(markdown) {
        var result = d.markdownConverter.makeHtml(markdown);
        var count = (result.match(/<p>/g) || []).length;
        if (count === 1 && result.startsWith("<p>") &&
                result.endsWith("</p>")) {
            result = result.substring(3, result.length - 4);
        }
        return result;
    }

    function strip(markdown) {
        return OTL.Application.htmlToPlainText(format(markdown));
    }

    QtObject {
        id: d

        property var markdownConverter: {
            var converter = new ShowdownJS.showdown.Converter();
            converter.setFlavor('github');
            // Note: Seens not to be supported by QML/Qt :(
            converter.setOption('strikethrough', true);
            converter.setOption('ghMentions', false);
            return converter;
        }
    }
}
