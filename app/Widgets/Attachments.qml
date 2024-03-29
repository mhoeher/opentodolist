import QtQuick 2.0
import QtQuick.Layouts 1.3

import OpenTodoList 1.0 as OTL

import "../Components"
import "../Controls" as C
import "../Fonts"
import "../Utils"
import "../Windows"
import "../Dialogs" as Dialogs

Item {
    id: item

    function attach() {
        dialog.open()
    }

    function attachFiles(fileUrls) {
        for (var i = 0; i < fileUrls.length; ++i) {
            item.item.attachFile(OTL.Application.urlFromString(fileUrls[i]))
        }
    }

    property OTL.ComplexItem item

    height: childrenRect.height

    OTL.ShareUtils {
        id: shareUtils
    }

    Dialogs.FilesDialog {
        id: dialog

        title: qsTr("Attach File")

        onAccepted: {
            for (var i = 0; i < selectedFiles.length; ++i) {
                item.item.attachFile(OTL.Application.urlFromString(
                                         selectedFiles[i]))
            }
        }
    }

    CenteredDialog {
        id: confirmDeleteAttachmentDialog

        property string attachment

        title: qsTr("Delete Attachment?")
        width: 400

        C.Label {
            text: qsTr("Are you sure you want to delete the attachment <strong>%1</strong>? This action " + "cannot be undone.").arg(
                      confirmDeleteAttachmentDialog.attachment)
            width: parent.width
        }
        standardButtons: C.Dialog.Ok | C.Dialog.Cancel
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
                onClicked: shareUtils.openFile(item.item.attachmentFileName(
                                                   modelData))

                RowLayout {
                    width: parent.width

                    C.Label {
                        text: modelData
                        Layout.fillWidth: true
                    }

                    C.ToolButton {
                        symbol: Icons.mdiDelete
                        hoverEnabled: true
                        onClicked: {
                            confirmDeleteAttachmentDialog.attachment = modelData
                            confirmDeleteAttachmentDialog.open()
                        }
                    }
                }
            }
        }
    }
}
