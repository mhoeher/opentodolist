import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Dialogs 1.2

import net.rpdev.OpenTodoList 1.0
import net.rpdev.OpenTodoList.UI 1.0

Item {
    id: page

    property ImageTopLevelItem item: ImageTopLevelItem {}
    property StackView stack: null
    property var library: null
    property bool __visible: Stack.status === Stack.Active

    function cancel() {
    }

    function deleteItem() {
        confirmDeleteDialog.open();
    }

    MessageDialog {
        id: confirmDeleteDialog
        title: qsTr("Delete Image?")
        text: qsTr("Are you sure you want to delete the image <strong>%1</strong>? This action " +
                   "cannot be undone.").arg(item.title)
        standardButtons: StandardButton.Ok | StandardButton.Cancel
        onAccepted: {
            item.deleteItem();
            stack.pop();
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

        horizontalScrollBarPolicy: Qt.ScrollBarAlwaysOff
        anchors.fill: parent

        Column {
            width: scrollView.flickableItem.width
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
                onClicked: {
                    var page = stack.push({ item: notesEditor, properties: { text: item.notes }});
                    page.onTextChanged.connect(function() { item.notes = page.text; });
                    item.onReloaded.connect(function() { page.text = item.notes; });
                }
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

                RichTextEditor {
                    Component.onCompleted: forceActiveFocus()
                }
            }
        }
    }
}




