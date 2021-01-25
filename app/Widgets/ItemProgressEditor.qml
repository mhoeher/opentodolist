import QtQuick 2.9
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.12

import OpenTodoList 1.0 as OTL

import "../Components"
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

    ToolButton {
        symbol: Icons.faPercent
        font.family: Fonts.icons
        background: Item {}
    }

    Slider {
        id: slider
        from: 0
        to: 100
        value: root.item.progress
        stepSize: 1
        live: false
        onValueChanged: root.item.progress = value
        Layout.fillWidth: true
    }

    ToolButton {
        symbol: Icons.faTimes
        font.family: Fonts.icons
        onClicked: {
            slider.value = -1;
            root.item.progress = -1;
        }
        visible: d.isProgressSet
    }
}
