import QtQuick 2.10
import QtQuick.Controls.Material 2.12
import QtQuick.Layouts 1.3
import Qt.labs.platform 1.0

import OpenTodoList 1.0 as OTL

import "../Components"
import "../Controls" as C
import "../Fonts"
import "../Pages"
import "../Utils"

Column {
    id: root

    property OTL.ComplexItem item: OTL.ComplexItem {}
    readonly property bool editing: !!loader.item
    property alias extraButton: extraButton
    property alias extraButton2: extraButton2

    function finishEditing() {
        if (loader.item) {
            loader.item.doneEditing();
        }
    }

    RowLayout {
        width: parent.width
        Heading {
            level: 2
            text: qsTr("Notes")
            Layout.fillWidth: true
        }

        C.ToolButton {
            id: extraButton2
            visible: false
            symbol: Icons.mdiDelete
        }

        C.ToolButton {
            id: extraButton
            visible: false
            symbol: Icons.mdiEdit
        }

        C.ToolButton {
            symbol: Icons.mdiSave
            visible: Qt.platform.os !== "ios" && Qt.platform.os !== "android"
            onClicked: saveNotesDialog.open()
        }

        C.ToolButton {
            symbol: Icons.mdiEditNote
            onClicked: mouseArea.clicked(undefined)
        }
    }

    Item {
        width: parent.width
        height: childrenRect.height
        visible: !loader.item

        MouseArea {
            id: mouseArea

            width: parent.width
            height: childrenRect.height
            implicitHeight: height
            onClicked: loader.sourceComponent = notesEditor

            C.Label {
                width: parent.width
                textFormat: Text.RichText
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                text: if (root.item) {
                          return Markdown.markdownToHtml(root.item.notes);
                      } else {
                          return ""
                      }
            }

            C.Label {
                width: parent.width
                text: qsTr("No notes added yet - click here to add some.")
                Material.foreground: Material.Grey
                visible: root.item.notes === ""
            }
        }
    }

    Loader {
        id: loader

        width: parent.width
        visible: !!loader.item
    }

    Component {
        id: notesEditor

        MarkdownEditor {
            id: editor

            item: root.item
            onDoneEditing: {
                loader.sourceComponent = undefined;
            }
        }
    }

    FileDialog {
        id: saveNotesDialog
        title: qsTr("Export to File...")
        folder: StandardPaths.writableLocation(StandardPaths.DocumentsLocation)
        defaultSuffix: ".md"
        nameFilters: [
            qsTr("Markdown files") + " (*.md)",
            qsTr("All files") + " (*)"
        ]
        fileMode: FileDialog.SaveFile
        onAccepted: OTL.Application.saveTextToFile(
                        file,
                        "# " + root.item.title + "\n\n" + root.item.notes
                        )
    }
}
