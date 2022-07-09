import QtQuick 2.9
import QtQuick.Controls.Material 2.0

import "../Components"
import "../Controls" as C
import "../Fonts"
import "../Utils"

import OpenTodoList 1.0 as OTL

CenteredDialog {
    id: dialog

    property var selectedColor

    implicitWidth: C.ApplicationWindow.contentItem.width * 0.9
    implicitHeight: C.ApplicationWindow.contentItem.height * 0.9
    standardButtons: C.Dialog.Ok | C.Dialog.Cancel | C.Dialog.Reset
    onReset: dialog.selectedColor = undefined

    header: C.ToolBar {
        background: Rectangle {
            color: dialog.selectedColor || "transparent"
            height: 10
        }
    }

    Flickable {
        id: flickable
        width: dialog.availableWidth
        height: dialog.availableHeight
        clip: true
        contentHeight: colorFlow.height + 50

        C.ScrollIndicator.vertical: C.ScrollIndicator {}

        Flow {
            id: colorFlow

            width: flickable.width
            spacing: AppSettings.mediumSpace

            Repeater {
                model: OTL.Colors.loadColors()
                delegate: C.Button {
                    Material.background: modelData.color
                    Material.foreground: {
                        if (modelData.color.hslLightness < 0.5) {
                            // This is a rather dark color, use a light color for the text
                            return Qt.rgba(1, 1, 1, 1)
                        } else {
                            // For light backgrounds, use a dark text color:
                            return Qt.rgba(0, 0, 0, 1)
                        }
                    }
                    Material.elevation: dialog.selectedColor === modelData.color ? 10 : 0
                    font.bold: dialog.selectedColor === modelData.color

                    text: modelData.name
                    onClicked: dialog.selectedColor = modelData.color
                }
            }
        }
    }
}
