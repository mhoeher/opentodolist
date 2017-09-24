import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3

import OpenTodoList 1.0
import OpenTodoList.UI 1.0

Page {
    id: page

    property WebDAVSynchronizer synchronizer: null

    signal closePage()

    implicitWidth: 200
    implicitHeight: 200


    footer: RowLayout {
        Item {
            Layout.fillWidth: true
        }
        Button {
            text: qsTr("Cancel")
            onClicked: page.closePage()
        }
        Button {
            text: qsTr("OK")
            onClicked: {
                page.synchronizer.username = username.text;
                page.synchronizer.password = password.text;
                page.synchronizer.save();
                App.saveSynchronizerSecrets(page.synchronizer);
                page.closePage();
            }
        }
    }

    ScrollView {
        width: parent.width
        height: parent.height

        Column {
            width: parent.width

            Text {
                text: qsTr("User name")
            }

            TextField {
                id: username
                width: parent.width
                text: page.synchronizer.username
            }

            Text {
                text: qsTr("Password")
            }

            TextField {
                id: password
                width: parent.width
                text: page.synchronizer.password
                echoMode: TextInput.Password
            }
        }
    }
}
