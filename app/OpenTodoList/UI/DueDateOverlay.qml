import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQml 2.2
import QtQuick.Layouts 1.3

import OpenTodoList.UI 1.0

Rectangle {
    id: overlay

    property date dueDate: new Date("")

    visible: dueDate.getTime() === dueDate.getTime()
    opacity: 0.8
    gradient: Gradient {
        GradientStop {
            position: 0.0
            color: "transparent"
        }
        GradientStop {
            position: 0.1
            color: Colors.light
        }
        GradientStop {
            position: 1.0
            color: Colors.midlight
        }
    }
    width: row.width + Globals.defaultMargin
    height: row.height + Globals.defaultMargin
    anchors {
        left: parent.left
        right: parent.right
        bottom: parent.bottom
        margins: 1
    }

    RowLayout {
        id: row
        spacing: Globals.defaultMargin
        anchors.centerIn: parent
        Symbol {
            symbol: Fonts.symbols.faCalendar

        }
        Label {
            text: overlay.dueDate.toLocaleDateString(
                      undefined, Locale.ShortFormat)
        }
    }
}
