import QtQuick 2.5
import QtQuick.Controls 2.2


import OpenTodoList 1.0
import OpenTodoList.UI 1.0

Page {
    id: page

    property ImageTopLevelItem item: ImageTopLevelItem {}
    property var library: null

    signal openPage(var component, var properties)
    signal closePage()

    function deleteItem() {
        confirmDeleteDialog.open();
    }

    CenteredDialog {
        id: confirmDeleteDialog

        title: qsTr("Delete Image?")

        Text {
            text: qsTr("Are you sure you want to delete the image <strong>%1</strong>? This action " +
                       "cannot be undone.").arg(item.title)
            width: 300
            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
        }

        standardButtons: Dialog.Ok | Dialog.Cancel
        x: (parent.width - width) / 2
        y: (parent.height - height) / 2
        onAccepted: {
            item.deleteItem();
            page.closePage();
        }
    }

    RenameItemDialog {
        id: renameItemDialog
    }

    Rectangle {
        color: Qt.lighter(Colors.itemColor(item.color), 1.1)
        opacity: 0.3
        anchors.fill: parent
    }

    ScrollView {
        id: scrollView

        anchors.fill: parent

        Column {
            width: scrollView.width
            spacing: Globals.defaultMargin

            MouseArea {
                onClicked: renameItemDialog.renameItem(item)
                height: childrenRect.height
                width: parent.width
                cursorShape: Qt.PointingHandCursor

                Label {
                    text: item.title
                    anchors {
                        left: parent.left
                        right: parent.right
                        margins: Globals.defaultMargin
                    }
                    font {
                        bold: true
                        pixelSize: Globals.fontPixelSize * 2
                    }
                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                }
            }

            Image {
                id: image
                visible: item.validImage
                source: item.imageUrl
                anchors {
                    left: parent.left
                    right: parent.right
                    margins: Globals.defaultMargin * 2
                }
                asynchronous: true
                fillMode: Image.PreserveAspectFit
            }

            StickyNote {
                id: note
                anchors {
                    left: parent.left
                    right: parent.right
                    margins: Globals.defaultMargin
                }
                title: qsTr("Notes")
                text: item.notes
                backgroundColor: item.color === TopLevelItem.White ? Colors.noteBackground : Colors.itemWhite
                onClicked: page.openPage(notesEditor, {item: page.item})
            }

            TagsEditor {
                item: page.item
                library: page.library
                anchors {
                    left: parent.left
                    right: parent.right
                    margins: Globals.defaultMargin * 2
                }
            }

            Item {
                height: Globals.defaultMargin
                anchors {
                    left: parent.left
                    right: parent.right
                    margins: Globals.defaultMargin * 2
                }
            }

            Component {
                id: notesEditor
                RichTextEditor {}
            }
        }
    }
}




