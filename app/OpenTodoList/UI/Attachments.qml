import QtQuick 2.0
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.2

import OpenTodoList 1.0
import OpenTodoList.UI 1.0


Item {
    id: item

    property ComplexItem item

    height: childrenRect.height

    OpenFileDialog {
        id: dialog

        title: qsTr("Attach File")
        parent: MainWindow.window.contentItem

        onAccepted: {
            item.item.attachFile(App.urlToLocalFile(fileUrl))
        }
    }

    CenteredDialog {
        id: confirmDeleteAttachmentDialog

        property string attachment

        title: qsTr("Delete Attachment?")
        Label {
            text: qsTr("Are you sure you want to delete the attachment <strong>%1</strong>? This action " +
                       "cannot be undone.").arg(confirmDeleteAttachmentDialog.attachment)
            wrapMode: Text.WordWrap
            anchors {
                left: parent.left
                right: parent.right
                top: parent.top
                margins: Globals.defaultMargin
            }
        }
        standardButtons: Dialog.Ok | Dialog.Cancel
        onAccepted: {
            item.item.detachFile(attachment)
        }
    }

    RowLayout {
        id: header
        width: parent.width

        Symbol {
            symbol: Fonts.symbols.faPaperclip
        }

        Label {
            font.bold: true
            text: qsTr("Attachments")
            Layout.fillWidth: true
        }

        Button {
            text: qsTr("Attach")
            onClicked: dialog.open()
        }
    }

    Column {
        anchors {
            left: parent.left
            right: parent.right
            top: header.bottom
            topMargin: Globals.defaultMargin
        }
        Repeater {
            model: item.item.attachments
            delegate: MouseArea {
                width: parent.width
                height: childrenRect.height + Globals.defaultMargin
                onClicked: Qt.openUrlExternally(
                               App.localFileToUrl(
                                   item.item.attachmentFileName(modelData)))
                RowLayout {
                    width: parent.width

                    Label {
                        text: modelData
                        Layout.fillWidth: true
                    }

                    Symbol {
                        symbol: Fonts.symbols.faTrashO
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
