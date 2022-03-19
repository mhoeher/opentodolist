import QtQuick 2.0
import QtQuick.Layouts 1.12
import OpenTodoList 1.0 as OTL
import "../Components"
import "../Utils"
import "../Fonts"

Item {
    id: root

    property int counter: 0
    property int total: 0
    property OTL.LibraryItem item: null

    width: parent.width
    height: childrenRect.height * 1.5

    Heading {
        id: pageHeading
        text: "<strong>%1</strong>%2".arg(
                  Markdown.markdownToHtml(root.item.title)).arg(
                  root.total > 0 ? " (%1/%2)".arg(root.counter).arg(root.total) :
                                     "")
        width: parent.width
        wrapMode: Text.WrapAtWordBoundaryOrAnywhere
        font.bold: false
        textFormat: Text.RichText
        bottomPadding: 0
        topPadding: 0
    }
}

