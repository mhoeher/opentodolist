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

    property bool useDenseVariant: false

    property int uiScaling: 100
    property bool overrideUiScaling: false

    readonly property int smallSpace: fontMetrics.height / 2
    readonly property int mediumSpace: smallSpace * 2
    readonly property int largeSpace: mediumSpace * 2

    readonly property FontMetrics effectiveFontMetrics: effFontMetrics

    Settings {
        property alias customFontSIze: settings.customFontSize
        property alias useCustomFontSize: settings.useCustomFontSize
        property alias useDenseVariant: settings.useDenseVariant
        property alias useCompactTodoLists: settings.useCompactTodoLists
        property alias uiScaling: settings.uiScaling
        property alias overrideUiScaling: settings.overrideUiScaling

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
