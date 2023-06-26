import QtQuick 2.15
import QtQuick.Layouts 1.0

import "../../Controls" as C
import OpenTodoList 1.0 as OTL
import "../../Utils" as Utils

C.ToolTip {
    id: tooltip

    property OTL.ComplexItem item: null

    delay: 100
    timeout: 10000
    y: parent.height - height * 1.5
    height: rowLayout.implicitHeight * 1.5
    visible: false

    RowLayout {
        id: rowLayout
        width: parent.availableWidth
        height: contentHeight

        Layout.alignment: Qt.AlignVCenter
        C.Label {
            text: qsTr("%1 is scheduled for the future - do you want to mark that future instance as done?").arg(
                      "<strong>" + Utils.Markdown.markdownToPlainText(
                          tooltip.item?.title) + "</strong>")
            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
            Layout.maximumWidth: Math.min(
                                     contentWidth,
                                     tooltip.parent.width * 0.9 - yesButton.width
                                     - noButton.width - spacing * 2)
        }

        C.Button {
            id: yesButton

            text: qsTr("Mark as Done")
            highlighted: true
            onClicked: () => {
                           tooltip.item.markCurrentOccurrenceAsDone()
                           tooltip.hide()
                       }
        }

        C.Button {
            id: noButton
            text: qsTr("Keep Open")
            onClicked: () => tooltip.hide()
        }
    }
}
