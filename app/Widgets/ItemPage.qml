import QtQuick 2.0
import QtQuick.Controls.Material 2.12

import OpenTodoList 1.0 as OTL

import "../Utils" as Utils
import "../Controls" as C
import "../Menues" as Menues

C.Page {
    property OTL.TopLevelItem topLevelItem: null

    property var selectColor: topLevelItem ? function () {
        colorMenu.popup()
    } : null

    Material.background: Utils.Colors.materialItemBackgroundColor(topLevelItem)

    Menues.ColorMenu {
        id: colorMenu
        item: topLevelItem
    }
}
