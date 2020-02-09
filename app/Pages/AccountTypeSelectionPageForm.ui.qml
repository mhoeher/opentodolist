import QtQuick 2.4
import QtQuick.Controls 2.5

import Components 1.0 as Components
import Fonts 1.0 as Fonts
import Utils 1.0 as Utils

Page {
    id: page

    property string selectedAccountType: ""
    property alias buttonBox: buttons

    signal accepted
    signal canceled

    title: qsTr("Select Account Type")

    footer: DialogButtonBox {
        id: buttons
        standardButtons: DialogButtonBox.Ok | DialogButtonBox.Cancel
    }

    ListView {
        anchors.fill: parent

        header: Components.Heading {
            text: qsTr("Account Type")
            width: parent.width
            padding: 10
        }
        model: accountsModel
        delegate: ItemDelegate {
            width: parent.width
            text: name
            down: page.selectedAccountType === type
            onClicked: page.selectedAccountType = type
        }
    }

    ListModel {
        id: accountsModel

        ListElement {
            type: "NextCloud"
            name: qsTr("NextCloud")
        }

        ListElement {
            type: "OwnCloud"
            name: qsTr("ownCloud")
        }

        ListElement {
            type: "WebDAV"
            name: qsTr("WebDAV")
        }
    }
}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/

