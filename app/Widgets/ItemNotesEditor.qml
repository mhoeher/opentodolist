import QtQuick 2.10
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12
import QtQuick.Layouts 1.3
import Qt.labs.platform 1.0

import OpenTodoList 1.0 as OTL

import "../Components"
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

        ToolButton {
            id: extraButton2
            visible: false
            symbol: Icons.faTrash
        }

        ToolButton {
            id: extraButton
            visible: false
            symbol: Icons.faPencilAlt
        }

        ToolButton {
            symbol: Icons.faShareAlt
            visible: Qt.platform.os !== "ios" && Qt.platform.os !== "android"
            onClicked: saveNotesDialog.open()
        }

        ToolButton {
            symbol: Icons.faICursor
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

            MarkdownLabel {
                id: markdownLabel

                width: parent.width
                textFormat: Text.RichText
                markdown: if (root.item) {
                              return root.item.notes
                          } else {
                              return ""
                          }
            }

            Label {
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
        visible: !!item
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
