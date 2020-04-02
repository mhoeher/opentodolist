import QtQuick 2.0
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12
import QtQuick.Layouts 1.12
import OpenTodoList 1.0 as OTL
import "../Components"
import "../Utils"
import "../Fonts"

Item {
    id: root

    property int counter: 0
    property OTL.Item item: null

    width: parent.width
    height: childrenRect.height * 2

    Heading {
        id: pageHeading
        text: "<strong>%1</strong>%2".arg(
                  Markdown.markdownToHtml(root.item.title)).arg(
                  root.counter > 0 ? " (%1)".arg(root.counter) :
                                     "")
        width: parent.width
        wrapMode: "WrapAtWordBoundaryOrAnywhere"
        font.bold: false
        textFormat: Text.RichText
    }

    RowLayout {
        id: pageMetaRow

        readonly property bool belowHeading: pageMetaRow.width +
                                             pageHeading.contentWidth +
                                             50 >
                                             page.width

        anchors.top: pageMetaRow.belowHeading ? pageHeading.bottom : parent.top
        anchors.right: parent.right
        anchors.verticalCenter: pageMetaRow.belowHeading ? undefined : pageHeading.verticalCenter

        Label {
            visible: DateUtils.validDate(root.item.dueTo)
            text: root.item.dueTo.toLocaleDateString()
            Layout.alignment: Qt.AlignVCenter
            Material.foreground: Material.Grey
        }

        ToolButton {
            symbol: Icons.faCalendarMinus
            visible: DateUtils.validDate(root.item.dueTo)
            Layout.alignment: Qt.AlignVCenter
            onClicked: root.item.dueTo = new Date("")
        }
    }
}

