import QtQuick 2.5

import OpenTodoList 1.0 as OTL

import "../Components"
import "../Utils"
import "../Fonts"


Column {
    id: editor

    property OTL.ComplexItem item: null

    signal doneEditing()

    QtObject {
        id: d

        function loadText() {
            textArea.text = item.notes;
            saveTimer.stop();
        }

        function saveText() {
            item.notes = textArea.text;
            saveTimer.stop();
        }
    }

    Component.onCompleted: {
        if (item !== null) {
            d.loadText();
        }
        textArea.forceActiveFocus();
    }
    Component.onDestruction: d.saveText()
    onItemChanged: d.loadText()

    Connections {
        target: item
        ignoreUnknownSignals: true
        onNotesChanged: textArea.text = item.notes
    }

    TextArea {
        id: textArea

        width: parent.width

        font.family: "Courier New, Courier, Fixed"
        padding: 20
        rightPadding: {
            switch (Qt.platform.os) {
            case "android":
                return 40;
            default:
                return padding;
            }
        }

        onTextChanged: saveTimer.start()
        onFocusChanged: if (!focus) {
                            editor.doneEditing()
                        }
    }

    Timer {
        id: saveTimer
        interval: 5000
        repeat: false
        onTriggered: d.saveText()
    }

    RoundButton {
        onClicked: editor.doneEditing()
        anchors.right: parent.right
        anchors.rightMargin: 10
        backgroundColor: Colors.positiveColor
        symbol: Icons.faCheck
    }

    OTL.SyntaxHighlighter {
        document: textArea.textDocument
    }
}
