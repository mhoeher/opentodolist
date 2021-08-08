import QtQuick 2.0
import QtQuick.Layouts 1.1
import QtQuick.Controls.Material 2.12

import OpenTodoList 1.0 as OTL

import "../Components"
import "../Controls" as C
import "../Fonts"
import "../Utils"
import "../Windows"

Column {
    id: editor
    
    property OTL.TopLevelItem item: null
    property var library: null

    function addTag() {
        newTagDialog.open();
    }
    
    width: parent.width

    CenteredDialog {
        id: newTagDialog

        Column {

            spacing: AppSettings.mediumSpace

            Heading {
                level: 2
                text: qsTr("Add Tag")
            }

            C.ComboBox {
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
        }

        standardButtons: C.Dialog.Ok | C.Dialog.Cancel
        onAccepted: {
            if (comboBox.editText !== "") {
                editor.item.addTag(comboBox.editText);
            }
            comboBox.editText = "";
        }
        onRejected: comboBox.editText = ""
    }

    Flow {
        width: parent.width
        spacing: AppSettings.smallSpace

        Repeater {
            model: editor.item.tags
            delegate: C.Frame {
                leftPadding: 10
                rightPadding: 0
                topPadding: 0
                bottomPadding: 0
                background: Rectangle {
                    id: tagBackground

                    color: Material.color(Material.Green, Material.Shade600)
                    border {
                        width: 1
                        color: Qt.lighter(tagBackground.color, 1.1)
                    }
                    radius: height / 3
                }
                Material.foreground: "white"

                RowLayout {
                    C.Label {
                        text: modelData
                        Layout.alignment: Qt.AlignVCenter
                    }
                    C.ToolButton {
                        symbol: Icons.mdiClose
                        onClicked: editor.item.removeTag(modelData)
                        Layout.alignment: Qt.AlignVCenter
                        background: Item {}
                    }
                }
            }
        }
    }
}
