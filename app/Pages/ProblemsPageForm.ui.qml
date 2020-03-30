import QtQuick 2.10
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.12

import Components 1.0 as Components

Page {
    id: page

    signal accountSelected(var account)
    property alias listView: listView

    title: qsTr("Problems Detected")

    ScrollView {
        anchors.fill: parent

        ListView {
            id: listView
            width: page.width
            height: page.height
            header: Components.Heading {
                text: qsTr("Problems Detected")
                padding: 10
                width: parent.width
            }
        }
    }
}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/

