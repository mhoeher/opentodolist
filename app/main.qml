import QtQuick 2.5
import QtQuick.Controls 1.4

import OpenTodoList.UI 1.0
import OpenTodoList 1.0

MainWindow {
    id: applicationWindow

    Component.onCompleted: console.debug(App.librariesLocation)
}
