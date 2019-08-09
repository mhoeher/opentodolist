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

        anchors {
            left: parent.left
            right: parent.right
            leftMargin: {
                switch (Qt.platform.os) {
                case "android":
                    return 50;
                default:
                    return 20;
                }
            }
            rightMargin: textArea.anchors.leftMargin
        }


        font.family: "Courier New, Courier, Fixed"

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
        font.family: Fonts.solidIcons
    }

    OTL.SyntaxHighlighter {
        document: textArea.textDocument
    }
}
