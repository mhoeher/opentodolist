import QtQuick 2.5
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12

import OpenTodoList 1.0 as OTL

import "../Components" as Components
import "../Utils"
import "../Fonts"


Column {
    id: editor

    property OTL.ComplexItem item: null

    signal doneEditing()

    QtObject {
        id: d

        property var savedTexts: ({})

        function loadText() {
            let hash = OTL.Application.sha256(item.notes);
            if (!savedTexts[hash]) {
                // We have not saved the text before, so assume this is a real update from the
                // server.
                textArea.text = item.notes;
            } else {
                // This is a "fake" update we caused on our own. Irgnore it but remove the
                // hash from out list.
                delete savedTexts[hash];
            }
            saveTimer.stop();
        }

        function saveText() {
            item.notes = textArea.text;
            saveTimer.stop();

            // Remember the currently saved text - the item will emit an update when we
            // auto saved and then a sync is triggered. This update would cause the text in
            // the editor to be discarded, which is certainly not what a user wants.
            // Note, that this is still only a poor man's sync solution for notes and might
            // still fail when multiple users work in parallel on the same note.
            savedTexts[OTL.Application.sha256(textArea.text)] = true;
        }
    }

    Component.onCompleted: {
        if (item !== null) {
            d.loadText();

            // Remember the initial item's text.
            d.savedTexts[OTL.Application.sha256(item.notes)] = true;
        }
        textArea.forceActiveFocus();
    }
    Component.onDestruction: d.saveText()
    onItemChanged: {
        d.loadText();

        // Remember the initial item's text.
        d.savedTexts = {};
        d.savedTexts[OTL.Application.sha256(item.notes)] = true;
    }

    Connections {
        target: item
        ignoreUnknownSignals: true
        function onNotesChanged() {
            d.loadText();
        }
    }

    Components.TextArea {
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

    Components.RoundButton {
        onClicked: editor.doneEditing()
        anchors.right: parent.right
        anchors.rightMargin: 10
        backgroundColor: Colors.positiveColor
        symbol: Icons.faCheck
        font.family: Fonts.solidIcons
    }

    OTL.SyntaxHighlighter {
        document: textArea.textDocument
        theme: Colors.syntaxHighlightingTheme
    }
}
