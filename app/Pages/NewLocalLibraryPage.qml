import QtQuick 2.4
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.0

import Components 1.0 as Components
import OpenTodoList 1.0 as OTL

Page {
    id: page

    property alias buttons: buttonBox
    property alias nameEdit: nameEdit

    signal closePage()
    signal libraryCreated(OTL.Library library)

    footer: DialogButtonBox {
        id: buttonBox
        standardButtons: DialogButtonBox.Ok | DialogButtonBox.Cancel
        onRejected: closePage()
        onAccepted: {
            if (nameEdit.displayText !== "") {
                var lib = OTL.Application.addLocalLibrary(nameEdit.displayText);
                if (lib) {
                    libraryCreated(lib);
                }
            }
        }
    }

    Component.onCompleted: {
        d.okButton = buttons.standardButton(DialogButtonBox.Ok);
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

    ScrollView {
        anchors.fill: parent
        padding: 10

        GridLayout {
            id: grid

            width: page.width - 2 * 10
            columns: 2

            Components.Heading {
                text: qsTr("Create a Local Library")
                Layout.fillWidth: true
                Layout.columnSpan: 2
            }

            Label {
                Layout.maximumWidth: grid.width
                Layout.columnSpan: 2
                text: qsTr("A local library is stored solely on your device - "
                           + "this makes it perfect for the privacy concise!" + "<br/><br/>"
                           + "Use it when you want to store information only "
                           + "locally and back up all your data regularly "
                           + "via other mechanisms. If you need to access your "
                           + "information across several devices, create a "
                           + "library which is synced instead.")
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
            }

            Label {
                text: qsTr("Name:")
            }

            TextField {
                id: nameEdit
                placeholderText: qsTr("My Local Library Name")
                Layout.fillWidth: true
            }
        }
    }
}
