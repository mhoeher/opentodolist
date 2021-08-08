import QtQuick 2.0
import QtQuick.Controls.Material 2.0

import OpenTodoList 1.0 as OTL

import "../Utils" as Utils
import "../Controls" as C

C.Pane {
    id: pane

    readonly property double lightShade: 0.1
    readonly property double midShade: 0.3
    readonly property double heavyShade: 0.5

    property OTL.TopLevelItem item: null
    property double shade: lightShade

    Material.background: Utils.Colors.materialItemBackgroundColor(item, shade)
}
