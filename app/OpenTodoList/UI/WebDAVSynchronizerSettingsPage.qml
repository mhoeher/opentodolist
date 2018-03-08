import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3

import OpenTodoList 1.0
import OpenTodoList.UI 1.0

Page {
    id: page

    property WebDAVSynchronizer synchronizer: null

    signal closePage()

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
        id: scrollView

        width: parent.width
        height: parent.height

        Column {
            width: scrollView.width

            Label {
                text: qsTr("User name")
            }

            TextField {
                id: username
                width: parent.width
                text: page.synchronizer.username
                inputMethodHints: Qt.ImhSensitiveData | Qt.ImhNoPredictiveText
                selectByMouse: true
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
            }

            Label {
                text: qsTr("Password")
            }

            TextField {
                id: password
                width: parent.width
                text: page.synchronizer.password
                echoMode: TextInput.Password
                inputMethodHints: Qt.ImhSensitiveData | Qt.ImhNoPredictiveText
                selectByMouse: true
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
            }
        }
    }
}
