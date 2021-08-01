import QtQuick 2.0

import "../Controls" as C
import "." as Components

C.Menu {
    property alias actions: repeater.model

    modal: true

    Repeater {
        id: repeater

        delegate: C.MenuItem {
            action: modelData
            visible: enabled
            height: visible ? implicitHeight : 0
        }
    }
}
