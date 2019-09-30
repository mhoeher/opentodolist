import QtQuick 2.0
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12

import OpenTodoList 1.0 as OTL

import "../Utils" as Utils

Pane {
    id: pane

    readonly property double lightShade: 0.1
    readonly property double midShade: 0.3
    readonly property double heavyShade: 0.5

    property OTL.TopLevelItem item: null
    property double shade: lightShade

    Material.background: Utils.Colors.materialItemBackgroundColor(item, shade)
}
