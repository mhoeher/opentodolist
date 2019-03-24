import QtQuick 2.5

import OpenTodoList 1.0 as OTL

import "../Components"


Page {
    id: editor
    
    property OTL.ComplexItem item: null

    property alias backgroundColor: editorBackground.backgroundColor
    
    signal accepted()
    
    function cancel() {
        textArea.focus = false;
        accepted();
    }

    title: qsTr("Edit Notes")

    Component.onCompleted: {
        if (item !== null) {
            textArea.text = item.notes;
        }
    }
    Component.onDestruction: {
        if (item !== null) {
            item.notes = textArea.text;
        }
    }
    onItemChanged: {
        if (item !== null) {
            textArea.text = item.notes;
        }
    }

    Connections {
        target: item
        ignoreUnknownSignals: true
        onNotesChanged: textArea.text = item.notes
    }

    Pane {
        id: editorBackground
        anchors.fill: parent
        padding: 0

        Flickable {
            id: flickable
            anchors.fill: parent
            flickableDirection: Flickable.VerticalFlick
            ScrollBar.vertical: ScrollBar {}
            contentHeight: textArea.height
            contentWidth: width

            MouseArea {
                width: flickable.width
                height: flickable.height
                onClicked: textArea.forceActiveFocus()
            }

            TextArea {
                id: textArea

                width: flickable.width
                padding: 5

                font.family: "Courier New, Courier, Fixed"

                Keys.onEscapePressed: focus = false
                Keys.onBackPressed: focus = false
            }

            OTL.SyntaxHighlighter {
                document: textArea.textDocument
            }
        }
    }
}
