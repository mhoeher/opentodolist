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
    property var pageActions: {[]}

    property alias sidebarControl: sidebarControl
    property alias backToolButton: backToolButton
    property alias problemsButton: problemsButton
    property alias busyIndicator: busyIndicator
    property list<ToolBarAction> dynamicPageActions

    function closeMenu() {
        pageMenuToolButton.menu.close()
    }


    leftPadding: 10
    rightPadding: 10

    QtObject {
        id: d

        property int numVisibleDynamicPageMenuItems: {
            return visibleDynamicPageActions.length -
                    numVisibleDynamicToolBarButtons;
        }

        property int numVisibleDynamicToolBarButtons: {
            var numButtons = widthForDynamicPageToolButtons /
                    (pageMenuToolButton.width + toolBarLayout.spacing);
            numButtons = parseInt(numButtons);
            // Cap to maximum number of available actions:
            numButtons = Math.min(numButtons, visibleDynamicPageActions.length);
            return numButtons;
        }

        property int widthForDynamicPageToolButtons: {
            // Calculate the space available for dynamic tool buttons:
            var availableWidth = toolBarLayout.width;
            // We need some space for the "static" buttons:
            availableWidth -=
                    sidebarControl.width * (sidebarControl.visible ? 1 : 0) +
                    problemsButton.width * (problemsButton.visible ? 1 : 0) +
                    backToolButton.width * (backToolButton.visible ? 1 : 0) +
                    busyIndicator.implicitWidth * (busyIndicator.visible ? 1 : 0) +
                    pageMenuToolButton.width;
            // Reserve some "minimum" space for the label:
            availableWidth -= Math.min(
                        fontMetrics.averageCharacterWidth * 30,
                        fontMetrics.boundingRect(pageTitleLabel.text).width);
            availableWidth -= 3 * toolBarLayout.spacing;

            // Cap to 0:
            return Math.max(availableWidth, 0);
        }

        property var visibleDynamicToolBarButtons
        property var visibleDynamicPageMenuItems
        // The list of all page actions which are visible
        property var visibleDynamicPageActions

        visibleDynamicPageActions: {
            var result = [];
            for (var i = 0; i < dynamicPageActions.length; ++i) {
                var action = dynamicPageActions[i];
                if (action.visible) {
                    result.push(action);
                }
            }
            return result;
        }

        visibleDynamicToolBarButtons: {
            var result = [];
            for (var i = 0; i < numVisibleDynamicToolBarButtons; ++i) {
                var action = visibleDynamicPageActions[i];
                result.push(action);
            }
            return result;
        }

        visibleDynamicPageMenuItems: {
            var result = [];
            for (var i = 0; i < numVisibleDynamicPageMenuItems; ++i) {
                var action = visibleDynamicPageActions[
                            numVisibleDynamicToolBarButtons + i];
                result.push(action);
            }
            return result;
        }
    }

    FontMetrics { id: fontMetrics; font: pageTitleLabel.font }

    RowLayout {
        id: toolBarLayout

        width: parent.width
        height: parent.height

        C.ToolButton {
            id: sidebarControl
            symbol: Fonts.Icons.mdiMenu
            Layout.alignment: Qt.AlignVCenter
        }

        C.ToolButton {
            id: backToolButton

            symbol: {
                switch (Qt.platform.os) {
                case "ios":
                case "macos":
                    return Fonts.Icons.mdiArrowBackIosNew;
                default:
                    return Fonts.Icons.mdiArrowBack;
                }
            }
            Layout.alignment: Qt.AlignVCenter
        }

        C.ToolButton {
            id: problemsButton

            symbol: Fonts.Icons.mdiReportProblem
            Material.foreground: Utils.Colors.negativeColor
            Layout.alignment: Qt.AlignVCenter
        }

        C.BusyIndicator {
            id: busyIndicator

            implicitHeight: backToolButton.height
            implicitWidth: implicitHeight
            hoverEnabled: true

            C.ToolTip {
                text: qsTr("Synchronizing library...")
            }

            C.Label {
                anchors.centerIn: parent
                text: "" + stackView.syncProgress + "%"
                visible: stackView.syncProgress >= 0
            }
        }

        C.Label {
            id: pageTitleLabel
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignVCenter
            wrapMode: Text.NoWrap

            elide: Text.ElideRight
            text: headerToolBar.title
        }

        Repeater {
            model: d.visibleDynamicToolBarButtons

            delegate: C.ToolButton {
                hoverEnabled: true
                action: modelData
                menu: action.menu
                symbol: action.symbol
                visible: action.visible
                Layout.alignment: Qt.AlignVCenter
                C.ToolTip.text: action.text
                C.ToolTip.visible: hovered && action.text !== ""
                C.ToolTip.delay: Utils.AppSettings.tooltipDelay
                C.ToolTip.timeout: Utils.AppSettings.tooltipTimeout
            }
        }

        C.ToolButton {
            id: pageMenuToolButton

            property int numPageActions: pageActions.length

            symbol: Fonts.Icons.mdiMoreVert
            visible: d.numVisibleDynamicPageMenuItems > 0 ||
                     numPageActions > 0
            Layout.alignment: Qt.AlignVCenter
            property var pageMenuEntries: {
                let result = [];
                let dynamicPageActions = d.visibleDynamicPageMenuItems;
                for (var i = 0; i < dynamicPageActions.length; ++i) {
                    let item = dynamicPageActions[i];
                    result.push({"type": "item", "item": item});
                }
                if (dynamicPageActions.length > 0 && pageActions.length > 0) {
                    result.push({"type": "separator"})
                }
                for (i = 0; i < pageActions.length; ++i) {
                    let item = pageActions[i];
                    result.push({"type": "item", "item": item});
                }
                return result;
            }

            onPageMenuEntriesChanged: {
                pageMenuEntriesModel.clear();
                for (var i = 0; i < pageMenuEntries.length; ++i) {
                    pageMenuEntriesModel.append(pageMenuEntries[i]);
                }
            }

            ListModel {
                id: pageMenuEntriesModel
                dynamicRoles: true
            }

            menu: C.Menu {
                y: headerToolBar.height
                modal: true
                Repeater {
                    model: pageMenuEntriesModel
                    delegate: DelegateChooser {
                        role: "type"

                        DelegateChoice {
                            roleValue: "separator"
                            C.MenuSeparator {}
                        }

                        DelegateChoice {
                            roleValue: "item"
                            C.MenuItem {
                                action: item
                                visible: action.enabled
                                height: visible ? implicitHeight : 0
                            }
                        }
                    }
                }
            }
        }
    }
}
