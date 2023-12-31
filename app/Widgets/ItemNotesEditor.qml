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
    property bool editing: false
    property alias extraButton: extraButton
    property alias extraButton2: extraButton2

    function finishEditing() {
        editing = false
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
            onClicked: loader.sourceComponent = notesEditor
        }
    }

    Loader {
        id: loader

        width: parent.width
        sourceComponent: editing ? notesEditor : notesViewer
    }

    Component {
        id: notesViewer

        C.TextArea {
            id: textEdit
            readOnly: true
            width: parent.width
            textFormat: Text.RichText
            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
            placeholderText: root.item?.notes
                             !== "" ? "" : qsTr(
                                          "No notes added yet - click here to add some.")
            C.ToolTip.text: hoveredLink
            C.ToolTip.delay: 500
            text: Markdown.markdownToHtml(root.item?.notes ?? "")

            onLinkActivated: link => {
                                 shareUtils.openLink(link)
                                 startEditingTimer.stop()
                             }

            onReleased: mouse => {
                            if (mouse.button === Qt.RightButton) {
                                notesContextMenu.popup()
                            } else {
                                if (selectedText === "") {
                                    startEditingTimer.start()
                                }
                            }
                        }

            Timer {
                id: startEditingTimer
                interval: 100
                repeat: false
                onTriggered: root.editing = true
            }

            C.Menu {
                id: notesContextMenu

                modal: true

                C.MenuItem {
                    text: qsTr("Copy")
                    onTriggered: OTL.Application.copyToClipboard(
                                     root.item?.notes ?? "")
                    enabled: textEdit.selectedText === ""
                }

                C.MenuItem {
                    text: qsTr("Copy Formatted Text")
                    onTriggered: {
                        if (textEdit.selectedText === "") {
                            OTL.Application.copyHtmlToClipboard(
                                        Markdown.markdownToHtml(
                                            root.item?.notes ?? ""))
                        } else {
                            OTL.Application.copyToClipboard(
                                        textEdit.getFormattedText(
                                            textEdit.selectionStart,
                                            textEdit.selectionEnd))
                        }
                    }
                }

                C.MenuItem {
                    text: qsTr("Copy Plain Text")
                    onTriggered: {
                        if (textEdit.selectedText === "") {
                            OTL.Application.copyToClipboard(
                                        Markdown.markdownToPlainText(
                                            root.item?.notes ?? ""))
                        } else {
                            OTL.Application.copyToClipboard(
                                        textEdit.selectedText)
                        }
                    }
                }
            }
        }
    }

    Component {
        id: notesEditor

        MarkdownEditor {
            id: editor

            item: root.item
            onDoneEditing: {
                root.editing = false
            }
        }
    }

    FileDialog {
        id: saveNotesDialog
        title: qsTr("Export to File...")
        folder: StandardPaths.writableLocation(StandardPaths.DocumentsLocation)
        defaultSuffix: ".md"
        nameFilters: [qsTr("Markdown files") + " (*.md)", qsTr(
                "All files") + " (*)"]
        fileMode: FileDialog.SaveFile
        onAccepted: OTL.Application.saveTextToFile(
                        file, "# " + root.item.title + "\n\n" + root.item.notes)
    }

    OTL.ShareUtils {
        id: shareUtils
    }
}
