import QtQuick 2.0
import QtQuick.Layouts 1.12
import OpenTodoList 1.0 as OTL
import "../Components"
import "../Utils"
import "../Fonts"

GridLayout {
    id: root

    property int counter: 0
    property OTL.Item item: null

    columns: pageHeadingRow.width + pageMetaRow.width + 10 >
             parent.width ? 2 : 3
    width: parent.width

    MarkdownConverter {
        id: titleText
        markdown: root.item.title
    }

    RowLayout {
        id: pageHeadingRow

        Layout.fillWidth: true

        Heading {
            id: pageHeading
            text: titleText.text
            width: Math.min(parent.width, contentWidth)
            wrapMode: "WrapAtWordBoundaryOrAnywhere"
        }

        Heading {
            visible: root.counter > 0
            text: "(" + root.counter + ")"
            font.bold: false
        }
    }

    Item {
        Layout.fillWidth: true
        height: 1
    }

    RowLayout {
        id: pageMetaRow
        anchors.right: parent.right

        Text {
            color: Colors.color(Colors.grey)
            visible: DateUtils.validDate(root.item.dueTo)
            text: root.item.dueTo.toLocaleDateString()
            Layout.alignment: Qt.AlignVCenter
        }

        ToolButton {
            symbol: Icons.faCalendarMinus
            visible: DateUtils.validDate(root.item.dueTo)
            Layout.alignment: Qt.AlignVCenter
            onClicked: root.item.dueTo = new Date("")
        }
    }
}
