import QtQuick 2.0
import QtQuick.Controls 2.5

import "." as Components

Menu {
    property alias actions: repeater.model

    modal: true

    Repeater {
        id: repeater

        delegate: MenuItem {
            action: modelData
            visible: enabled
            height: visible ? implicitHeight : 0
        }
    }
}
