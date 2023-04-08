pragma Singleton

import QtQuick 2.10
import Qt.labs.settings 1.0

import OpenTodoList 1.0 as OTL

Item {
    id: settings

    property bool useCustomFontSize: false
    property int customFontSize: 12
    readonly property int defaultFontSize: fontMetrics.font.pointSize
    property bool useCompactTodoLists: false
    property bool showNotesExcepts: false // Show first line of notes in item listings
    readonly property int effectiveFontSize: useCustomFontSize ? customFontSize : defaultFontSize
    property int libraryItemWidthScaleFactor: 40

    property bool desktopMode: {
        switch (Qt.platform.os) {
            case "ios":
            case "android":
            return false
            default:
            return true
        }
    }

    property bool useDenseVariant: false

    property int uiScaling: 100
    property bool overrideUiScaling: false

    readonly property int smallSpace: fontMetrics.height / 2
    readonly property int mediumSpace: smallSpace * 2
    readonly property int largeSpace: mediumSpace * 2

    readonly property int tooltipDelay: 500
    readonly property int tooltipTimeout: 10000

    property font defaultFont: fontUtils.systemFont(OTL.FontUtils.GeneralFont)
    property font fixedFont: fontUtils.systemFont(OTL.FontUtils.FixedFont)

    property bool showQuickNotesEditorOnSystemTrayClick: false

    readonly property bool supportsQuickEditor: {
        switch (Qt.platform.os) {
            case "android":
            case "ios":
            // Not supported on these platforms - sorry
            return false
            default:
            return true
        }
    }

    readonly property bool selectTextByMouse: {
        switch (Qt.platform.os) {
            case "android":
            case "ios":
            return false
            default:
            return true
        }
    }

    readonly property FontMetrics effectiveFontMetrics: effFontMetrics

    // Expose sub-settings
    property alias todoListPageSettings: todoListPageSettings

    Settings {
        property alias customFontSIze: settings.customFontSize
        property alias useCustomFontSize: settings.useCustomFontSize
        property alias useDenseVariant: settings.useDenseVariant
        property alias useCompactTodoLists: settings.useCompactTodoLists
        property alias showNotesExcepts: settings.showNotesExcepts
        property alias uiScaling: settings.uiScaling
        property alias overrideUiScaling: settings.overrideUiScaling
        property alias libraryItemWidthScaleFactor: settings.libraryItemWidthScaleFactor
        property alias showQuickNotesEditorOnSystemTrayClick: settings.showQuickNotesEditorOnSystemTrayClick
        property alias desktopMode: settings.desktopMode

        category: "ApplicationWindow"
    }

    Settings {
        id: todoListPageSettings
        category: "TodoListPage"

        property bool showUndone: false
        property bool groupDone: false
        property string sortTodosBy: "weight"
    }

    FontMetrics {
        id: fontMetrics
    }

    FontMetrics {
        id: effFontMetrics

        font.pointSize: settings.effectiveFontSize
    }

    OTL.FontUtils {
        id: fontUtils
    }
}
