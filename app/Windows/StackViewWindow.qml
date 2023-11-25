import QtQuick

import "../Controls" as C
import "../Components" as Cmp
import "../Widgets" as W

C.ApplicationWindow {
    id: window

    property alias stackView: stackView

    function openInitialPage(page, props) {
        stackView.clear()
        stackView.push(page, props)
    }

    width: 800
    height: 600
    title: qsTr("OpenTodoList") + " - " + applicationVersion

    header: Cmp.ApplicationToolBar {
        id: applicationToolBar

        stackView: stackView

        appMenu: appMenu
        appShortcuts: applicationShortcuts
        sidebarControl.visible: false

        backToolButton.visible: stackView?.depth > 1
        backToolButton.onClicked: stackView.goBack()
        problemsButton.visible: false

        Binding {
            target: applicationToolBar
            property: "title"
            value: stackView?.currentItem?.title ?? ""
        }
    }

    C.StackView {
        id: stackView

        anchors.fill: parent
    }

    W.ApplicationMenu {
        id: appMenu
        shortcuts: applicationShortcuts
    }

    Cmp.ApplicationShortcuts {
        id: applicationShortcuts

        stackView: stackView
        window: window
        isSecondaryWindow: true

        leftSidebar.enabled: false
    }
}
