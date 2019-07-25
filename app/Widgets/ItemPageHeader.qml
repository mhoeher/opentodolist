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

    columns: pageHeading.width + pageMetaRow.width + 10 >
             parent.width ? 1 : 2
    width: parent.width

    MarkdownConverter {
        id: titleText
        markdown: root.item.title
    }

    Heading {
        id: pageHeading
        text: "<strong>%1</strong>%2".arg(
                  titleText.text).arg(
                  root.counter > 0 ? " (%1)".arg(root.counter) :
                                     "")
        width: Math.min(parent.width, contentWidth)
        wrapMode: "WrapAtWordBoundaryOrAnywhere"
        font.bold: false
        Layout.fillWidth: true
    }

    RowLayout {
        id: pageMetaRow

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
