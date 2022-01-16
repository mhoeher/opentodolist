pragma Singleton

import QtQuick 2.10
import QtQuick.Controls.Material 2.12
import Qt.labs.settings 1.0

import OpenTodoList 1.0 as OTL

Item {
    id: root

    readonly property string systemTheme: "system"
    readonly property string lightTheme: "light"
    readonly property string darkTheme: "dark"
    readonly property string systemThemeName: qsTr("System")
    readonly property string lightThemeName: qsTr("Light")
    readonly property string darkThemeName: qsTr("Dark")

    readonly property var themes: ([
                                             { "name": systemTheme, "title": systemThemeName },
                                             { "name": lightTheme, "title": lightThemeName },
                                             { "name": darkTheme, "title": darkThemeName },
                                         ])


    readonly property var materialBackground: Material.background
    readonly property int materialTheme: Material.theme

    readonly property bool usingDarkColorTheme: {
        if (theme === lightTheme) {
            return false;
        } else if (theme === darkTheme) {
            return true;
        } else {
            if (materialTheme === Material.Dark) {
                return true;
            } else {
                return false;
            }
        }
    }

    readonly property int syntaxHighlightingTheme: {
        if (usingDarkColorTheme) {
            return OTL.SyntaxHighlighter.Dark;
        } else {
            return OTL.SyntaxHighlighter.Light;
        }
    }

    property string theme: systemTheme

    Material.theme: {
        switch (theme) {
            case lightTheme: return Material.Light;
            case darkTheme: return Material.Dark;
            case systemTheme: return Material.System;
        }
    }

    // Pre-defined colors for specific "roles":
    readonly property int positiveColor: Material.Green
    readonly property int negativeColor: Material.Red
    readonly property color linkColor: {
        if (usingDarkColorTheme) {
            return Material.color(Material.LightGreen);
        } else {
            return Material.color(Material.Teal);
        }
    }

    // Pre-defined shades:
    readonly property int shade50: Material.Shade50
    readonly property int shade100: Material.Shade100

    function color(base, shade) {
        if (shade !== undefined) {
            return Material.color(base, shade);
        } else {
            return Material.color(base);
        }
    }

    function textColor(base, shade) {
        let backgroundColor = color(base, shade);
        if (backgroundColor.hslLightness > 0.5) {
            // The background is rather light... use a dark color for text:
            return Qt.rgba(0, 0, 0, 1);
        } else {
            // The background is dark, use a light color for text:
            return Qt.rgba(1, 1, 1, 1);
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

    function materialItemBackgroundColor(item, shade=0.1) {
        var result = materialBackground;
        if (item !== null) {
            var c = itemColor(item);
            c = Material.color(c);
            c.a = shade;
            result = Qt.tint(result, c);
        }
        return result;
    }


    Settings {
        category: "Colors"

        property alias theme: root.theme
    }
}
