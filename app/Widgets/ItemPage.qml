import QtQuick 2.0
import QtQuick.Controls.Material 2.12

import OpenTodoList 1.0 as OTL

import "../Utils" as Utils
import "../Controls" as C
import "../Menues" as Menues
import "../Windows" as W

C.Page {
    property OTL.TopLevelItem topLevelItem: null

    property var selectColor: {
        if (topLevelItem) {
            return function () {
                colorMenu.popup()
            }
        } else {
            return null
        }
    }

    function openStackViewWindow(page, props) {
        rootObject.newStackViewWindow(page, props)
    }

    Material.background: Utils.Colors.materialItemBackgroundColor(topLevelItem)

    Menues.ColorMenu {
        id: colorMenu
        item: topLevelItem
    }

    Component {
        id: stackViewWindow

        W.StackViewWindow {}
    }
}
