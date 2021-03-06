pragma Singleton

import QtQuick 2.10
import Qt.labs.settings 1.0

Item {
    id: settings

    property bool useCustomFontSize: false
    property int customFontSize: 12
    readonly property int defaultFontSize: fontMetrics.font.pointSize
    property bool useCompactTodoLists: false
    readonly property int effectiveFontSize: useCustomFontSize ? customFontSize : defaultFontSize
    property int libraryItemWidthScaleFactor: 40

    property bool useDenseVariant: false

    property int uiScaling: 100
    property bool overrideUiScaling: false

    readonly property int smallSpace: fontMetrics.height / 2
    readonly property int mediumSpace: smallSpace * 2
    readonly property int largeSpace: mediumSpace * 2

    readonly property int tooltipDelay: 500
    readonly property int tooltipTimeout: 10000

    property bool showQuickNotesEditorOnSystemTrayClick: false

    readonly property bool supportsQuickEditor: {
        switch (Qt.platform.os) {
            case "android":
            case "ios":
                // Not supported on these platforms - sorry
                return false;
            default:
                return true;
        }
    }

    readonly property bool selectTextByMouse: {
        switch (Qt.platform.os) {
        case "android":
        case "ios":
            return false;
        default:
            return true;
        }
    }

    readonly property FontMetrics effectiveFontMetrics: effFontMetrics

    Settings {
        property alias customFontSIze: settings.customFontSize
        property alias useCustomFontSize: settings.useCustomFontSize
        property alias useDenseVariant: settings.useDenseVariant
        property alias useCompactTodoLists: settings.useCompactTodoLists
        property alias uiScaling: settings.uiScaling
        property alias overrideUiScaling: settings.overrideUiScaling
        property alias libraryItemWidthScaleFactor: settings.libraryItemWidthScaleFactor
        property alias showQuickNotesEditorOnSystemTrayClick: settings.showQuickNotesEditorOnSystemTrayClick

        category: "ApplicationWindow"
    }

    FontMetrics {
        id: fontMetrics
    }

    FontMetrics {
        id: effFontMetrics

        font.pointSize: settings.effectiveFontSize
    }
}
