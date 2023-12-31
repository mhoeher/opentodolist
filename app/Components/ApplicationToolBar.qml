import QtQuick 2.12
import QtQuick.Layouts 1.0
import QtQuick.Controls.Material 2.0
import Qt.labs.qmlmodels 1.0

import "../Controls" as C
import "../Utils" as Utils
import "../Fonts" as Fonts
import "../Widgets" as Widgets

import OpenTodoList 1.0 as OTL

C.ToolBar {
    id: headerToolBar

    property string title: ""
    property C.StackView stackView
    property Widgets.ApplicationMenu appMenu
    property ApplicationShortcuts appShortcuts

    property alias sidebarControl: sidebarControl
    property alias backToolButton: backToolButton
    property alias problemsButton: problemsButton

    FontMetrics {
        id: fontMetrics
        font: pageTitleLabel.font
    }

    Column {
        width: parent.width
        RowLayout {
            id: toolBarLayout

            width: parent.width

            C.ToolButton {
                id: sidebarControl
                symbol: appShortcuts.leftSidebar.symbol
                down: appShortcuts.leftSidebar.checked
                onClicked: appShortcuts.leftSidebar.triggered()
                Layout.alignment: Qt.AlignVCenter
            }

            C.ToolButton {
                id: backToolButton

                symbol: {
                    switch (Qt.platform.os) {
                    case "ios":
                    case "macos":
                        return Fonts.Icons.mdiArrowBackIosNew
                    default:
                        return Fonts.Icons.mdiArrowBack
                    }
                }
                Layout.alignment: Qt.AlignVCenter
            }

            C.Label {
                id: pageTitleLabel
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignVCenter
                wrapMode: Text.NoWrap
                leftPadding: 10
                rightPadding: 10

                elide: Text.ElideRight
                text: headerToolBar.title
            }

            C.ToolButton {
                id: problemsButton

                symbol: Fonts.Icons.mdiReportProblem
                Material.foreground: Utils.Colors.negativeColor
                Layout.alignment: Qt.AlignVCenter

                C.ToolTip.text: qsTr("Problems")
                C.ToolTip.visible: Utils.AppSettings.desktopMode && hovered
                C.ToolTip.delay: Utils.AppSettings.tooltipDelay
                C.ToolTip.timeout: Utils.AppSettings.tooltipTimeout
            }

            Repeater {
                model: appMenu.menus
                delegate: C.ToolButton {
                    //hoverEnabled: true
                    symbol: modelData.symbol
                    menu: C.Menu {
                        id: menuFromAppMenu

                        property var appMenuEntry: modelData

                        title: modelData.title
                        modal: true
                        y: applicationToolBar.height

                        Repeater {
                            model: menuFromAppMenu.appMenuEntry.items
                            delegate: DelegateChooser {
                                role: "type"

                                DelegateChoice {
                                    roleValue: "separator"
                                    C.MenuSeparator {
                                        visible: false
                                        height: 0
                                    }
                                }

                                DelegateChoice {
                                    roleValue: "item"
                                    C.MenuItem {
                                        text: modelData.text
                                        onTriggered: modelData.triggered()
                                        enabled: modelData.enabled
                                        visible: enabled
                                        height: visible ? implicitHeight : 0
                                    }
                                }
                            }
                        }
                    }
                    Layout.alignment: Qt.AlignVCenter
                    C.ToolTip.text: modelData.title
                    C.ToolTip.visible: Utils.AppSettings.desktopMode && hovered
                                       && modelData.title !== ""
                    C.ToolTip.delay: Utils.AppSettings.tooltipDelay
                    C.ToolTip.timeout: Utils.AppSettings.tooltipTimeout
                }
            }
        }

        C.ProgressBar {
            id: progressBar

            width: parent.width
            from: 0
            to: 100
            value: {
                if (stackView?.syncRunning) {
                    // If a sync is running, report the current sync prograss as-is:
                    return stackView?.syncProgress ?? 0
                } else {
                    // Otherwise, always return 0:
                    return 0
                }
            }

            indeterminate: {
                if (stackView?.syncRunning ?? false) {
                    return (stackView?.syncProgress ?? 0) < 0
                } else {
                    // Also show indeterminate progress when cache accesses are queued:
                    return OTL.Application.cache.numberOfRunningTransactions > 0 ? 1.0 : 0.0
                }
            }
        }
    }
}
