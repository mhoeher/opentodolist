import QtQuick 2.9
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12

import OpenTodoList 1.0 as OTL

import "../Components"

Pane {
    id: root

    readonly property var syncErrors: {
        if (library) {
            return OTL.Application.syncErrors[
                    library.directory] || [];
        } else {
            return [];
        }
    }

    property bool shown: syncErrors.length > 0
    property OTL.Library library: null

    signal showErrors()

    Material.background: Material.Orange
    anchors {
        left: parent.left
        right: parent.right
    }
    y: shown ? parent.height - height : parent.height

    RowLayout {
        id: layout
        anchors {
            verticalCenter: parent.verticalCenter
            left: parent.left
            right: parent.right
        }

        Label {
            text: qsTr("There were errors when synchronizing the library. " +
                       "Please ensure that the library settings are up to date.")
            Layout.fillWidth: true
            wrapMode: Text.WordWrap
        }
        Button {
            text: qsTr("Ignore")
            onClicked: OTL.Application.clearSyncErrors(root.library)
        }
        Button {
            text: qsTr("View")
            onClicked: root.showErrors()
        }
    }
}

