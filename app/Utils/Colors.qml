pragma Singleton

import QtQuick 2.10
import QtQuick.Controls.Material 2.3

import OpenTodoList 1.0 as OTL

Item {
    // Pre-defined colors:
    readonly property int red: Material.Red
    readonly property int pink: Material.Pink
    readonly property int purple: Material.Purple
    readonly property int deepPurple: Material.DeepPurple
    readonly property int indigo: Material.Indigo
    readonly property int blue: Material.Blue
    readonly property int lightBlue: Material.LightBlue
    readonly property int cyan: Material.Cyan
    readonly property int teal: Material.Teal
    readonly property int green: Material.Green
    readonly property int lightGreen: Material.LightGreen
    readonly property int lime: Material.Lime
    readonly property int yellow: Material.Yellow
    readonly property int amber: Material.Amber
    readonly property int orange: Material.Orange
    readonly property int deepOrange: Material.DeepOrange
    readonly property int brown: Material.Brown
    readonly property int grey: Material.Grey
    readonly property int blueGrey: Material.BlueGrey

    // Pre-defined colors for specific "roles":
    readonly property int positiveColor: green
    readonly property int negativeColor: red

    // Pre-defined shades:
    readonly property int shade50: Material.Shade50
    readonly property int shade100: Material.Shade100
    readonly property int shade200: Material.Shade200
    readonly property int shade300: Material.Shade300
    readonly property int shade400: Material.Shade400
    readonly property int shade500: Material.Shade500
    readonly property int shade600: Material.Shade600
    readonly property int shade700: Material.Shade700
    readonly property int shade800: Material.Shade800
    readonly property int shade900: Material.Shade900
    readonly property int shadeA100: Material.ShadeA100
    readonly property int shadeA200: Material.ShadeA200
    readonly property int shadeA400: Material.ShadeA400
    readonly property int shadeA700: Material.ShadeA700

    function color(base, shade) {
        if (shade !== undefined) {
            return Material.color(base, shade);
        } else {
            return Material.color(base);
        }
    }

    function itemColor(item) {
        if (!item || item.color === undefined) {
            return "#00000000";
        }
        switch (item.color) {
        case OTL.TopLevelItem.White:
            return Material.Grey;
        case OTL.TopLevelItem.Red:
            return Material.Red;
        case OTL.TopLevelItem.Green:
            return Material.Green;
        case OTL.TopLevelItem.Blue:
            return Material.Blue;
        case OTL.TopLevelItem.Yellow:
            return Material.Yellow;
        case OTL.TopLevelItem.Orange:
            return Material.Orange;
        case OTL.TopLevelItem.Lilac:
            return Material.Purple;
        }
    }
}
