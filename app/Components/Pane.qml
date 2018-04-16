import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Controls.Material 2.3

import OpenTodoList 1.0 as OTL

Pane {
    id: pane

    // Pre-defined background colors per "type":
    readonly property color info: Material.color(
                                           Material.Green, Material.Shade100)
    readonly property color nonInteractive: Material.color(
                                           Material.Grey)
    readonly property color warn: Material.color(
                                           Material.Red, Material.Shade100)

    function mixColor(base, shade) {
        return Material.color(base, shade);
    }

    property var backgroundColor: undefined
    property var elevation: undefined

    Material.background: backgroundColor

    Component.onCompleted: {
        if (elevation !== undefined) {
            Material.elevation = elevation;
        }
    }
    onElevationChanged: {
        if (elevation !== undefined) {
            Material.elevation = elevation;
        } else {
            Material.elevation = undefined;
        }
    }
}
