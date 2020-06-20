import QtQuick 2.0
import QtQuick.Controls 2.0

import "../Widgets" as Widgets
import "." as Components

Repeater {
    delegate: Components.ToolButton {
        action: modelData
        symbol: modelData.symbol
        visible: !modelData.hideButton && enabled
    }
}
