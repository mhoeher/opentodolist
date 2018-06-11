pragma Singleton

import QtQuick 2.0

import OpenTodoList 1.0 as OTL

Item {
    id: root

    readonly property string stylesheet: "
    <style type='text/css'>
        a, a:visited {
            color: %1;
            text-decoration: none;
        }
    </style>
    ".arg(Colors.color(Colors.linkColor))

    function scheduleFormat(text) {
        worker.sendMessage({ text: text });
    }

    signal markdownReady(string text, string result)

    WorkerScript {
        id: worker

        source: "markdown-formatter-worker.js"
        onMessage: root.markdownReady(messageObject.text, messageObject.result)
    }
}
