import QtQuick 2.4
import QtQuick.Layouts 1.0

import "../Components" as Components
import "../Controls" as C
import "../Utils" as Utils

import OpenTodoList 1.0 as OTL

C.Page {
    id: page

    property alias buttons: buttonBox
    property alias nameEdit: nameEdit

    signal closePage
    signal libraryCreated(var library)

    footer: C.DialogButtonBox {
        id: buttonBox
        standardButtons: C.DialogButtonBox.Ok | C.DialogButtonBox.Cancel
        onRejected: closePage()
        onAccepted: {
            if (nameEdit.displayText !== "") {
                var lib = OTL.Application.addLocalLibrary(nameEdit.displayText)
                if (lib) {
                    libraryCreated(lib)
                }
            }
        }
    }

    Component.onCompleted: {
        d.okButton = buttons.standardButton(C.DialogButtonBox.Ok)
    }

    QtObject {
        id: d

        property var okButton
    }

    Binding {
        target: d.okButton
        property: "enabled"
        value: nameEdit.displayText !== ""
    }

    C.ScrollView {
        anchors.fill: parent
        padding: Utils.AppSettings.mediumSpace

        GridLayout {
            id: grid

            width: page.width - 2 * 10
            columns: 2
            implicitWidth: childrenRect.width
            implicitHeight: childrenRect.height

            Components.Heading {
                text: qsTr("Create a Local Library")
                Layout.fillWidth: true
                Layout.columnSpan: 2
            }

            C.Label {
                Layout.maximumWidth: grid.width
                Layout.columnSpan: 2
                text: qsTr("A local library is stored solely on your device - "
                           + "this makes it perfect for the privacy concise!" + "<br/><br/>"
                           + "Use it when you want to store information only "
                           + "locally and back up all your data regularly "
                           + "via other mechanisms. If you need to access your "
                           + "information across several devices, create a "
                           + "library which is synced instead.")
            }

            C.Label {
                text: qsTr("Name:")
            }

            C.TextField {
                id: nameEdit
                placeholderText: qsTr("My Local Library Name")
                Layout.fillWidth: true
            }
        }
    }
}
