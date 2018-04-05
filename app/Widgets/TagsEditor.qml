import QtQuick 2.0
import QtQuick.Layouts 1.1

import OpenTodoList 1.0 as OTL

import "../Components"
import "../Fonts"
import "../Utils"
import "../Windows"

Column {
    id: editor
    
    property OTL.TopLevelItem item: null
    property var library: null
    
    width: parent.width

    CenteredDialog {
        id: newTagDialog

        ComboBox {
            id: comboBox

            model: {
                var library = editor.library;
                var result = [];
                if (library) {
                    for (var i = 0; i < library.tags.length; ++i) {
                        var attached = false;
                        for (var j = 0; j < item.tags.length; ++j) {
                            if (library.tags[i] === item.tags[j]) {
                                attached = true;
                                break;
                            }
                        }
                        if (!attached) {
                            result.push(library.tags[i]);
                        }
                    }
                }
                return result;
            }
            editable: true
        }
        standardButtons: Dialog.Ok | Dialog.Cancel
        onAccepted: {
            if (comboBox.editText !== "") {
                editor.item.addTag(comboBox.editText);
            }
            comboBox.editText = "";
        }
        onRejected: comboBox.editText = ""
    }
    
    RowLayout {
        width: parent.width

        ToolButton {
            id: decorativeIcon
            symbol: Icons.faTags
            background: Item {}
        }

        Label {
            Layout.fillWidth: true
            text: qsTr("Tags")
            font.bold: true
        }

        ToolButton {
            symbol: Icons.faPlus
            onClicked: newTagDialog.open()
        }
    }

    Flow {
        x: decorativeIcon.width
        width: parent.width - x
        spacing: 5

        Repeater {
            model: editor.item.tags
            delegate: Frame {
                leftPadding: 10
                rightPadding: 0
                topPadding: 0
                bottomPadding: 0
                background: Rectangle {
                    id: tagBackground
                    color: Colors.color(Colors.green, Colors.shade200)
                    border {
                        width: 1
                        color: Qt.darker(tagBackground.color)
                    }
                    radius: height / 2
                }
                RowLayout {
                    Label {
                        text: modelData
                        Layout.alignment: Qt.AlignVCenter
                    }
                    ToolButton {
                        symbol: Icons.faTimes
                        onClicked: editor.item.removeTag(modelData)
                        Layout.alignment: Qt.AlignVCenter
                        background: Item {}
                    }
                }
            }
        }
    }
}
