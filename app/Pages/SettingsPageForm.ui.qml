import QtQuick 2.4
import QtQuick.Controls 2.5
import "../Components"
import QtQuick.Layouts 1.3

Page {
    id: page

    property alias themeEdit: themeEdit

    signal close

    width: 400
    height: 600
    property alias languageEdit: languageEdit
    padding: 10
    footer: DialogButtonBox {
        id: buttons

        standardButtons: DialogButtonBox.Close
    }

    ScrollView {
        id: scrollView
        anchors.fill: parent

        GridLayout {
            width: scrollView.width
            columns: 2

            Heading {
                text: qsTr("Settings")
                Layout.fillWidth: true
                Layout.columnSpan: 2
            }

            Heading {
                text: qsTr("User Interface")
                level: 2
                Layout.fillWidth: true
                Layout.columnSpan: 2
            }

            Label {
                id: label
                text: qsTr("Language:")
            }

            ComboBox {
                id: languageEdit
                Layout.fillWidth: true
            }

            Label {
                text: qsTr("Theme:")
            }

            ComboBox {
                id: themeEdit
                Layout.fillWidth: true
            }
        }
    }

    Connections {
        target: buttons
        onRejected: page.close()
    }
}

/*##^##
Designer {
    D{i:2;anchors_height:113;anchors_width:175}
}
##^##*/

