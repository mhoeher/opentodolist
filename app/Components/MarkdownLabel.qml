import QtQuick 2.10

import "../Utils"

Label {
    property alias markdown: d.markdown

    text: {
        if (textFormat === Text.RichText) {
            return Markdown.stylesheet + d.text;
        } else {
            return d.text;
        }
    }

    MarkdownConverter { id: d }
}
