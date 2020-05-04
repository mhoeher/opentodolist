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
    height: childrenRect.height * 1.5

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
}

