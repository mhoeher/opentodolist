import QtQuick 2.4
import QtQuick.Controls 2.13

import Components 1.0 as Components
import Fonts 1.0 as Fonts
import Utils 1.0 as Utils

Page {
    id: page

    property alias accounts: listView.model
    property alias roundButton: roundButton

    signal accountSelected(var account)

    title: qsTr("Accounts")

    ListView {
        id: listView

        anchors.fill: parent

        header: Components.Heading {
            text: qsTr("Accounts")
            padding: 10
        }
        model: defaultData
        delegate: ItemDelegate {
            id: itemDelegate

            text: name
            width: parent.width

            Connections {
                target: itemDelegate
                onClicked: page.accountSelected(account)
            }
        }
    }

    ListModel {
        id: defaultData

        ListElement {
            name: "Account 1"
        }

        ListElement {
            name: "Account 2"
        }

        ListElement {
            name: "Account 3"
        }

        ListElement {
            name: "Account 4"
        }

        ListElement {
            name: "Account 5"
        }

        ListElement {
            name: "Account 6"
        }

        ListElement {
            name: "Account 7"
        }

        ListElement {
            name: "Account 8"
        }

        ListElement {
            name: "Account 9"
        }

        ListElement {
            name: "Account 10"
        }

        ListElement {
            name: "Account 11"
        }

        ListElement {
            name: "Account 12"
        }

        ListElement {
            name: "Account 13"
        }
    }

    Components.RoundButton {
        id: roundButton
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.margins: 10
        symbol: Fonts.Icons.faPlus
        backgroundColor: Utils.Colors.positiveColor
    }
}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/

