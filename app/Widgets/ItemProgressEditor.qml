import QtQuick 2.9
import QtQuick.Layouts 1.3

import OpenTodoList 1.0 as OTL

import "../Components"
import "../Controls" as C
import "../Fonts"
import "../Utils"
import "../Windows"

RowLayout {
    id: root

    property OTL.Todo item: null

    QtObject {
        id: d

        property bool isProgressSet: root.item.progress >= 0
    }

    C.ToolButton {
        symbol: Icons.mdiFactCheck
        font.family: Fonts.icons
        background: Item {}
    }

    C.Slider {
        id: slider
        from: 0
        to: 100
        value: root.item.progress
        stepSize: 1
        live: false
        onValueChanged: if (d.isProgressSet) { root.item.progress = value; }
        Layout.fillWidth: true
    }

    C.ToolButton {
        symbol: Icons.mdiDelete
        font.family: Fonts.icons
        onClicked: {
            slider.value = -1;
            root.item.progress = -1;
        }
        visible: d.isProgressSet
    }
}
