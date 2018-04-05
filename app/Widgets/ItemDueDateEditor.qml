import QtQuick 2.9
import QtQuick.Layouts 1.3

import OpenTodoList 1.0 as OTL

import "../Components"
import "../Fonts"
import "../Utils"
import "../Windows"

RowLayout {
    id: root

    property OTL.ComplexItem item: null

    QtObject {
        id: d

        property bool validDate: DateUtils.validDate(root.item.dueTo)
    }

    ToolButton {
        symbol: d.validDate ? Icons.faCalendarAlt : Icons.faCalendar
        font.family: Fonts.icons
        background: Item {}
    }

    Label {
        Layout.fillWidth: true
        text: d.validDate ? qsTr("Due on") + " " +
                            root.item.dueTo.toLocaleDateString() : ""
        font.bold: true
    }

    ToolButton {
        symbol: Icons.faCalendarPlus
        font.family: Fonts.icons
        onClicked: {
            dialog.selectedDate = root.item.dueTo;
            dialog.open();
        }
    }

    ToolButton {
        symbol: Icons.faCalendarTimes
        font.family: Fonts.icons
        onClicked: root.item.dueTo = new Date("")
        visible: d.validDate
    }

    DateSelectionDialog {
        id: dialog
        onAccepted: root.item.dueTo = selectedDate
    }
}
