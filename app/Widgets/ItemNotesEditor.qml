import QtQuick 2.10
import QtQuick.Layouts 1.3

import OpenTodoList 1.0 as OTL

import "../Components"
import "../Fonts"
import "../Pages"
import "../Utils"

Column {
    id: root

    property OTL.ComplexItem item: OTL.ComplexItem {}

    RowLayout {
        width: parent.width

        ToolButton {
            id: decorativeItem
            background: Item {}
            symbol: Icons.faStickyNote
            font.family: Fonts.icons
        }

        Label {
            font.bold: true
            text: qsTr("Notes")
            Layout.fillWidth: true
        }

        ToolButton {
            symbol: Icons.faPencilAlt
            onClicked: mouseArea.clicked(undefined)
        }
    }

    Frame {
        width: parent.width - x
        x: decorativeItem.width

        MouseArea {
            id: mouseArea

            width: parent.width
            height: childrenRect.height
            implicitHeight: height
            onClicked: page.openPage(notesEditor, {"item": item})

            Label {
                width: parent.width
                textFormat: Text.RichText
                text: Markdown.stylesheet + Markdown.format(page.item.notes)
            }
        }
    }

    Component {
        id: notesEditor

        MarkdownEditorPage {
            id: editor

            backgroundColor: background.backgroundColor
        }
    }
}
