pragma Singleton

import QtQuick 2.9

import OpenTodoList 1.0

QtObject {
    id: colors

    // New style attributes
    readonly property color shadow: palette.shadow
    readonly property color window: palette.window
    readonly property color windowText: palette.windowText
    readonly property color light: palette.light
    readonly property color midlight: palette.midlight
    readonly property color mid: palette.mid
    readonly property color dark: palette.dark
    readonly property color highlight: palette.highlight
    readonly property color highlightedText: palette.highlightedText
    readonly property color base: palette.base
    readonly property color darkText: {
        var dark = colors.dark;
        var avg = (dark.r + dark.g + dark.b) / 3;
        if (avg < 0.5) {
            return "white";
        } else {
            return "black";
        }
    }

    readonly property color accentText: Qt.tint(
                                            palette.text,
                                            makeTransparent(primary, 0.5))
    readonly property color infoBackground: Qt.tint(
                                            palette.window,
                                            makeTransparent(primary, 0.5))
    readonly property color warnBackground: Qt.tint(
                                            palette.window,
                                            makeTransparent(secondary1, 0.5))


    // TODO: Remove these and rather switch to palette colors?
    property color primary: "#56ad23"
    property color secondary1: "#ad2356"
    property color secondary2: "#ad4f23"
    property color secoUndary3: "#2388ad"
    
    property color itemWhite: "#fefefe"
    property color itemRed: "#dea7bb"
    property color itemGreen: "#cce6bd"
    property color itemBlue: "#a7cfde"
    property color itemOrange: "#ffab7f"
    property color itemYellow: "#eee8aa"
    property color itemLilac: "#9683cf"

    readonly property SystemPalette palette: SystemPalette {}
    readonly property SystemPalette inactive: SystemPalette {
        colorGroup: SystemPalette.Inactive
    }
    
    function itemColor(color) {
        var pureColor = pureTtemColor(color);
        return Qt.tint(mid, makeTransparent(pureColor, 0.4));
    }

    function lightItemColor(color) {
        var pureColor = pureTtemColor(color);
        return Qt.tint(light, makeTransparent(pureColor, 0.4));
    }

    function pureTtemColor(color) {
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
