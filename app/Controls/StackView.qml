import QtQuick 2.10
import QtQuick.Controls 2.3 as QQC2
import Qt.labs.settings 1.0 as Settings

QQC2.StackView {
    id: stack


    /**
     * @brief A unique ID used to store the persistent data of the page stack.
     */
    property string stackId: ""


    /**
     * @brief If set, when upon restoring the stack is empty, create an instance of this component.
     */
    property var startPageComponent

    QtObject {
        id: d

        property bool initialized: false
    }

    Connections {
        target: stack
        function onCurrentItemChanged() {
            if (d.initialized && stackId != "") {
                let state = []
                for (var i = 0; i < stack.depth; ++i) {
                    let page = stack.get(i)
                    let pageUrl = page.restoreUrl
                    if (pageUrl && pageUrl.toString() !== "") {
                        state.push({
                                       "url": pageUrl,
                                       "state": page.savePage()
                                   })
                    } else {
                        break
                    }
                }
                settings.setValue("state", JSON.stringify(state))
            }
        }
    }

    Component.onCompleted: {
        try {
            if (stack.stackId !== "") {
                let state = null
                try {
                    state = JSON.parse(settings.value("state", "[]"))
                } catch (exc) {
                    console.warn("Exception caught during stack state restoration:",
                                 exc)
                    state = []
                }
                for (var i = 0; i < state.length; ++i) {
                    let pageState = state[i]
                    let page = stack.push(Qt.resolvedUrl(pageState.url))
                    page.restorePage(pageState.state)
                }
            }
        } catch (e) {
            console.warn("Error during restoring stack: ", e)
        }
        try {
            if (stack.depth === 0 && startPageComponent) {
                stack.push(startPageComponent)
            }
        } catch (e2) {
            console.warn("Error during creating start page: ", e2)
        }

        d.initialized = true
    }

    Settings.Settings {
        id: settings
        category: "StackView"
    }
}
