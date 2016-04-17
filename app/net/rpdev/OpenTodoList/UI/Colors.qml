pragma Singleton

import QtQuick 2.0

import net.rpdev.OpenTodoList 1.0

QtObject {
    
    property color primary: "#56ad23"
    property color secondary1: "#ad2356"
    property color secondary2: "#ad4f23"
    property color secondary3: "#2388ad"
    
    property color panelDark: Qt.darker(secondary3, 3)
    property color panelLight: Qt.darker(secondary3, 2)
    property color panelText: "white"
    
    property color noteBackground: "#eef6e9"
    
    property color headerBar: primary
    
    property color itemDelimiter: "silver"
    
    property color tagBackground: Qt.tint("white", makeTransparent(primary, 0.1))
    property color tagBorder: "silver"
    property color tagText: "black"
    
    property color itemWhite: "#fefefe"
    property color itemRed: "#dea7bb"
    property color itemGreen: "#cce6bd"
    property color itemBlue: "#a7cfde"
    property color itemOrange: "#ffab7f"
    property color itemYellow: "#eee8aa"
    property color itemLilac: "#9683cf"
    
    function itemColor(color) {
        switch (color) {
        case TopLevelItem.White: return itemWhite;
        case TopLevelItem.Red: return itemRed;
        case TopLevelItem.Green: return itemGreen;
        case TopLevelItem.Blue: return itemBlue;
        case TopLevelItem.Orange: return itemOrange;
        case TopLevelItem.Yellow: return itemYellow;
        case TopLevelItem.Lilac: return itemLilac;
        default: return itemWhite;
        }
    }
    
    function makeTransparent(color, opacity) {
        return Qt.rgba(color.r, color.g, color.b, opacity)
    }

}
