import QtQuick 2.0
import QtQuick.Layouts 1.1

import OpenTodoList 1.0 as OTL

import "../Components"
import "../Windows"
import "../Controls" as C

Item {
    id: root

    property OTL.Library library
    readonly property var actions: {
        let result = []
        if (syncNowAction.enabled) {
            result.push(syncNowAction)
        }
        if (syncLogAction.enabled) {
            result.push(syncLogAction)
        }
        return result
    }

    signal openPage(var component, var properties)

    C.Action {
        id: syncNowAction

        text: qsTr("Sync Now")
        enabled: root.library && root.library.hasSynchronizer
        onTriggered: {
            console.debug("Manually started syncing " + root.library.name)
            OTL.Application.syncLibrary(library)
        }
    }

    C.Action {
        id: syncLogAction

        text: qsTr("Sync Log")
        enabled: root.library && root.library.hasSynchronizer
        onTriggered: root.openPage(Qt.resolvedUrl("../Pages/LogViewPage.qml"), {
                                       "log": root.library.syncLog()
                                   })
    }
}
