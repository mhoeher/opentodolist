import QtQuick

import "./Windows" as Windows

QtObject {
    id: rootObject

    property Windows.MainWindow mainWindow: Windows.MainWindow {
        visible: true
    }

    function newStackViewWindow(page, props) {
        var window = stackViewWindow.createObject(rootObject)
        window.openInitialPage(page, props)
        window.show()
        window.raise()
        secondaryWindows.push(window)
    }

    property var stackViewWindow: Component {
        Windows.StackViewWindow {
            id: stackViewWin

            onVisibilityChanged: {
                if (stackViewWin.visibility === Window.Hidden) {
                    let windows = []
                    secondaryWindows.forEach(window => {
                                                 if (window !== stackViewWin) {
                                                     windows.push(window)
                                                 }
                                             })
                    secondaryWindows = windows
                }
            }
        }
    }

    property var secondaryWindows: []
}
