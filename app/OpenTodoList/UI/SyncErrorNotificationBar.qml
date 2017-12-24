import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3

import OpenTodoList 1.0
import OpenTodoList.UI 1.0


Rectangle {
    id: root

    property bool shown: library !== null && library.syncErrors.length > 0
    property Library library: null

    signal showErrors()

    anchors {
        left: parent.left
        right: parent.right
    }
    height: layout.height + Globals.defaultMargin
    color: Colors.warnBackground
    y: shown ? parent.height - height : parent.height

    RowLayout {
        id: layout
        anchors {
            verticalCenter: parent.verticalCenter
            left: parent.left
            right: parent.right
            margins: Globals.defaultMargin
        }

        Label {
            text: qsTr("There were errors when synchronizing the library. " +
                       "Please ensure that the library settings are up to date.")
            Layout.fillWidth: true
            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
        }
        Button {
            text: qsTr("Ignore")
            onClicked: root.library.clearSyncErrors()
        }
        Button {
            text: qsTr("View")
            onClicked: root.showErrors()
        }
    }
}

