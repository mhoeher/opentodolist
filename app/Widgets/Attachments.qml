import QtQuick 2.0
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.12

import OpenTodoList 1.0 as OTL

import "../Components"
import "../Fonts"
import "../Utils"
import "../Windows"


Item {
    id: item

    property OTL.ComplexItem item
    readonly property OpenFileDialog openFileDialog: dialog

    function attach() {
        dialog.open();
    }

    height: childrenRect.height

    OpenFileDialog {
        id: dialog

        title: qsTr("Attach File")
        parent: MainWindow.overlay

        onAccepted: {
            item.item.attachFile(OTL.Application.urlToLocalFile(fileUrl))
        }
    }

    CenteredDialog {
        id: confirmDeleteAttachmentDialog

        property string attachment

        title: qsTr("Delete Attachment?")
        width: 400

        Label {
            text: qsTr("Are you sure you want to delete the attachment <strong>%1</strong>? This action " +
                       "cannot be undone.").arg(confirmDeleteAttachmentDialog.attachment)
            width: parent.width
        }
        standardButtons: Dialog.Ok | Dialog.Cancel
        onAccepted: {
            item.item.detachFile(attachment)
        }
    }

    Heading {
        id: header

        level: 2
        text: qsTr("Attachments")
        visible: item.item.attachments.length > 0
    }

    Column {
        anchors {
            left: parent.left
            right: parent.right
            top: header.bottom
        }
        Repeater {
            model: item.item.attachments
            delegate: MouseArea {
                width: parent.width
                height: childrenRect.height
                onClicked: OTL.Application.openUrl(OTL.Application.localFileToUrl(
                                                       item.item.attachmentFileName(modelData)))

                RowLayout {
                    width: parent.width

                    Label {
                        text: modelData
                        Layout.fillWidth: true
                    }

                    ToolButton {
                        symbol: Icons.faTrash
                        hoverEnabled: true
                        onClicked: {
                            confirmDeleteAttachmentDialog.attachment = modelData;
                            confirmDeleteAttachmentDialog.open();
                        }
                    }
                }
            }
        }
    }
}
