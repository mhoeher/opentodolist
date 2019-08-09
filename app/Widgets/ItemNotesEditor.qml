import QtQuick 2.10
import QtQuick.Layouts 1.3

import OpenTodoList 1.0 as OTL

import "../Components"
import "../Fonts"
import "../Pages"
import "../Utils"

Column {
    id: root

    property OTL.ComplexItem item: OTL.ComplexItem {}
    readonly property bool editing: !!loader.item

    function finishEditing() {
        loader.item.doneEditing();
    }


    RowLayout {
        width: parent.width
        Heading {
            level: 2
            text: qsTr("Notes")
            Layout.fillWidth: true
        }

        ToolButton {
            symbol: Icons.faPencilAlt
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
            //onClicked: page.openPage(notesEditor, {"item": item})
            onClicked: loader.sourceComponent = notesEditor

            MarkdownLabel {
                width: parent.width
                textFormat: Text.RichText
                markdown: page.item.notes
            }

            Label {
                width: parent.width
                text: qsTr("No notes added yet - click here to add some.")
                color: Colors.color(Colors.grey)
                visible: page.item.notes === ""
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
}
