import QtQuick 2.0
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.1

import OpenTodoList 1.0 as OTL

import "../Components"
import "../Windows"

Menu {
    id: menu

    property OTL.Library library

    signal openPage(var component, var properties)

    modal: true
    
    MenuItem {
        text: qsTr("Sync Now")
        enabled: menu.library.hasSynchronizer
        onClicked: {
            console.debug("Manually started syncing " + menu.library.name);
            OTL.Application.syncLibrary(library);
        }
    }
    
    MenuItem {
        text: qsTr("Sync Log")
        enabled: menu.library.hasSynchronizer
        onClicked: menu.openPage(
                       Qt.resolvedUrl("../Pages/LogViewPage.qml"),
                       {"log": menu.library.syncLog()})
    }
}
