import QtQuick 2.10
import QtQuick.Controls 2.3 as QQC2

QQC2.MenuItem {
    onTriggered: {
        if (action && action.menu) {
            action.menu.popup()
        }
    }
}
