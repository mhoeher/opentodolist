import QtQuick 2.10

import OpenTodoList 1.0 as OTL

import "../Utils"

QtObject {
    id: converter

    property string markdown: ""
    property bool strip: false
    readonly property alias text: d.text
    property list<QtObject> __children

    __children: [
        Connections {
            target: Markdown
            onMarkdownReady: {
                if (text === converter.markdown) {
                    if (converter.strip) {
                        d.text = OTL.Application.htmlToPlainText(result);
                    } else {
                        d.text = result;
                    }
                }
            }
        },
        QtObject {
            id: d

            property string text
        }
    ]

    onMarkdownChanged: Markdown.scheduleFormat(markdown)
    onStripChanged: Markdown.scheduleFormat(markdown)
}
