import QtQuick 2.10
import QtQuick.Window 2.0
import QtQuick.Layouts 1.0
import QtQuick.Controls 2.5 as QQC2
import Qt.labs.settings 1.0

import "../Components" as Components
import "../Utils" as Utils
import "../Fonts" as Fonts

import OpenTodoList 1.0 as OTL

Window {
    id: root

    signal openMainWindow()

    title: qsTr("Quick Notes")
    width: 400
    height: 600

    onVisibleChanged: if (visible) {
                          noteText.forceActiveFocus();
                      }

    QQC2.Page {

        anchors.fill: parent
        padding: Utils.AppSettings.smallSpace

        header: QQC2.ToolBar {
            padding: Utils.AppSettings.smallSpace
            RowLayout {
                anchors.fill: parent

                Components.Label {
                    text: qsTr("Quick Notes Editor")
                    Layout.fillWidth: true
                    elide: "ElideRight"
                }

                Components.ToolButton {
                    symbol: Fonts.Icons.faFolderOpen
                    hoverEnabled: true
                    onClicked: root.openMainWindow()
                    QQC2.ToolTip.delay: Utils.AppSettings.tooltipDelay
                    QQC2.ToolTip.timeout: Utils.AppSettings.tooltipTimeout
                    QQC2.ToolTip.visible: hovered
                    QQC2.ToolTip.text: qsTr("Open the main window")
                }
            }
        }

        GridLayout {
            anchors.fill: parent
            columns: 2

            Components.TextField {
                id: noteTitle

                placeholderText: qsTr("Quick Note Title")
                Layout.fillWidth: true
                Layout.columnSpan: 2
            }

            QQC2.ScrollView {
                Layout.fillHeight: true
                Layout.fillWidth: true
                Layout.columnSpan: 2

                Components.TextArea {
                    id: noteText

                    placeholderText: qsTr("Type your notes here...")
                }
            }

            QQC2.ComboBox {
                id: librarySelector

                Layout.fillWidth: true
                model: OTL.LibrariesModel {
                    cache: OTL.Application.cache
                }
                textRole: "name"
                valueRole: "library"
                onCurrentValueChanged: {
                    if (currentValue !== null) {
                        settings.lastLibraryUid = currentValue.uid;
                    }
                }
                onCountChanged: {
                    if (settings.lastLibraryUid) {
                        for (var i = 0; i < count; ++i) {
                            let lib = model.get(i);
                            if (lib.uid === settings.lastLibraryUid) {
                                currentIndex = i;
                                return;
                            }
                        }
                    }
                }
            }

            QQC2.Button {
                id: saveButton

                function getCreateArgs(defaultTitlePrefix) {
                    let title = noteTitle.text;
                    if (title === "") {
                        title = defaultTitlePrefix + " - " + (new Date()).toLocaleString();
                    };
                    return {
                        "title": title,
                        "notes": noteText.text
                    };
                }

                function clear() {
                    noteTitle.clear();
                    noteText.clear();
                }

                text: qsTr("Save")
                enabled: !!librarySelector.currentValue && noteText.text !== ""
                onClicked: createNoteAction.trigger()
                onPressAndHold: contextMenu.popup()
                hoverEnabled: true
                QQC2.ToolTip.delay: Utils.AppSettings.tooltipDelay
                QQC2.ToolTip.timeout: Utils.AppSettings.tooltipTimeout
                QQC2.ToolTip.visible: hovered
                QQC2.ToolTip.text: qsTr("Save the entered notes to the selected library. " +
                                        "Press and hold the button to get more options for saving.")

                QQC2.Menu {
                    id: contextMenu

                    modal: true

                    QQC2.MenuItem { action: createNoteAction }
                    QQC2.MenuItem { action: createTodoListAction }
                }
            }
        }
    }

    QQC2.Action {
        id: createNoteAction
        text: qsTr("Save as Note")
        enabled: saveButton.enabled
        onTriggered: {
            OTL.Application.addNote(librarySelector.currentValue, saveButton.getCreateArgs(qsTr("Quick Note")));
            saveButton.clear();
        }
        shortcut: StandardKey.Save
    }

    QQC2.Action {
        id: createTodoListAction
        text: qsTr("Save as Todo List")
        onTriggered: {
            OTL.Application.addTodoList(librarySelector.currentValue, saveButton.getCreateArgs(qsTr("Quick Todo List")));
            saveButton.clear();
        }
    }

    QQC2.Action {
        shortcut: StandardKey.Close
        onTriggered: root.hide()
    }

    QQC2.Action {
        shortcut: StandardKey.Quit
        onTriggered: Qt.quit()
    }

    OTL.SyntaxHighlighter {
        document: noteText.textDocument
        theme: Utils.Colors.syntaxHighlightingTheme
    }

    Settings {
        id: settings

        category: "QuickNotesEditor"

        property alias noteTitle: noteTitle.text
        property alias noteText: noteText.text
        property var lastLibraryUid
        property alias width: root.width
        property alias height: root.height
    }
}
