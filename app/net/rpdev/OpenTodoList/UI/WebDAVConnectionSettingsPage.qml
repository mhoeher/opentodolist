import QtQuick 2.0
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.3

import net.rpdev.OpenTodoList 1.0
import net.rpdev.OpenTodoList.UI 1.0

Item {
    id: page

    signal connectionDataAvailable(var synchronizer)
    signal cancelled()

    property var synchronizer: ({})

    QtObject {
        id: d

    }

    WebDAVSynchronizer {
        id: dav

        serverType: synchronizer.serverType
        url: url.text
        username: username.text
        password: password.text
        disableCertificateCheck: ignoreSslErrors.checked
    }

    ScrollView {
        anchors {
            left: parent.left
            right: parent.right
            top: parent.top
            bottom: buttons.top
        }

        Flickable {
            anchors.fill: parent

            Column {
                anchors {
                    left: parent.left
                    right: parent.right
                    margins: Globals.defaultMargin
                }

                Item {
                    height: Globals.defaultMargin
                    width: 1
                }
                Label {
                    text: qsTr("Server Address:")
                    width: parent.width
                }
                TextField {
                    id: url
                    placeholderText: qsTr("https://...")
                    width: parent.width
                }

                Item {
                    height: Globals.defaultMargin
                    width: 1
                }
                Label {
                    text: qsTr("User name:")
                    width: parent.width
                }
                TextField {
                    id: username
                    width: parent.width
                }

                Item {
                    height: Globals.defaultMargin
                    width: 1
                }
                Label {
                    text: qsTr("Password:")
                    width: parent.width
                }
                TextField {
                    id: password
                    echoMode: TextInput.Password
                    width: parent.width
                }

                Item {
                    height: Globals.defaultMargin
                    width: 1
                }
                CheckBox {
                    id: ignoreSslErrors
                    text: qsTr("Ignore SSL Errors")
                    checked: false
                }
            }
        }
    }

    RowLayout {
        id: buttons

        anchors {
            bottom: parent.bottom
            left: parent.left
            right: parent.right
            margins: Globals.defaultMargin
        }
        spacing: Globals.defaultMargin
        layoutDirection: Qt.RightToLeft

        Button {
            text: qsTr("Validate")
            visible: !dav.valid
            onClicked: dav.validate()
        }

        Button {
            text: qsTr("OK")
            visible: dav.valid
        }
        Button {
            text: qsTr("Cancel")
            onClicked: page.cancelled()
        }
        Item {
            height: 1
            width: 1
            Layout.fillWidth: true
        }
    }

}
