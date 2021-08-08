import QtQuick 2.0

import "../Widgets" as Widgets
import "." as Components
import "../Controls" as C

Repeater {
    delegate: C.ToolButton {
        action: modelData
        symbol: modelData.symbol
        visible: !modelData.hideButton && enabled
    }
}
