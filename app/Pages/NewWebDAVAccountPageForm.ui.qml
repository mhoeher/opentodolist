import QtQuick 2.4
import QtQuick.Controls 2.5
import QtQuick.Controls.Material 2.0
import QtQuick.Layouts 1.0

import Components 1.0 as Components
import Utils 1.0 as Utils

Page {
    id: page

    property string type: ""
    property alias buttons: buttons
    property alias serverAddressEdit: serverAddressEdit
    property alias usernameEdit: usernameEdit
    property alias accountNameEdit: accountNameEdit
    property alias errorLabel: errorLabel
    property alias disableCertificateChecksEdit: disableCertificateChecksEdit
    property alias scrollView: scrollView
    property alias busyIndicator: busyIndicator
    property alias passwordEdit: passwordEdit

    title: qsTr("Connection Settings")
    footer: DialogButtonBox {
        id: buttons

        standardButtons: DialogButtonBox.Ok | DialogButtonBox.Cancel
    }

    ScrollView {
        id: scrollView

        anchors.fill: parent
        padding: 10

        GridLayout {
            width: scrollView.contentItem.width
            columns: 2
            columnSpacing: 10
            rowSpacing: 10

            Components.Heading {
                text: qsTr("Connection Settings")
                Layout.columnSpan: 2
                Layout.fillWidth: true
            }

            Components.Label {
                text: qsTr("Name:")
            }

            TextField {
                id: accountNameEdit

                Layout.fillWidth: true
            }

            Components.Label {
                text: qsTr("Server Address:")
            }

            TextField {
                id: serverAddressEdit

                inputMethodHints: Qt.ImhNoAutoUppercase | Qt.ImhUrlCharactersOnly
                Layout.fillWidth: true
            }

            Components.Label {
                text: qsTr("User:")
            }

            TextField {
                id: usernameEdit

                inputMethodHints: Qt.ImhNoAutoUppercase | Qt.ImhNoPredictiveText
                Layout.fillWidth: true
            }

            Components.Label {
                text: qsTr("Password:")
            }

            TextField {
                id: passwordEdit

                Layout.fillWidth: true
                echoMode: TextInput.Password
                inputMethodHints: Qt.ImhSensitiveData | Qt.ImhNoPredictiveText
            }

            Item {
                width: 1
                height: 1
            }

            CheckBox {
                id: disableCertificateChecksEdit

                text: qsTr("Disable Certificate Checks")
            }

            Components.Label {
                id: errorLabel

                Layout.columnSpan: 2
                Layout.fillWidth: true
                text: qsTr("Failed to connect to the server. Please "
                           + "check your user name, password and the server address and retry.")
                Material.foreground: Material.Red
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
            }
        }
    }

    BusyIndicator {
        id: busyIndicator

        anchors.centerIn: parent
        visible: false
    }
}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/

