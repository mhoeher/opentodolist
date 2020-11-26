import QtQuick 2.9
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.5

import OpenTodoList 1.0 as OTL

import "../Components"
import "../Fonts"
import "../Utils"
import "../Windows"

GridLayout {
    id: root

    property OTL.ComplexItem item: null

    columns: 3
    visible: DateUtils.validDate(item.dueTo)

    QtObject {
        id: d

        property bool validDate: DateUtils.validDate(root.item.dueTo)
    }

    Heading {
        level: 2
        text: qsTr("Due on") + " " + root.item.effectiveDueTo.toLocaleDateString()
        wrapMode: "WrapAtWordBoundaryOrAnywhere"
        Layout.fillWidth: true
    }

    ToolButton {
        symbol: d.validDate ? Icons.faCalendarMinus : ""
        font.family: Fonts.icons
        background: Item {}
        onClicked: root.item.dueTo = new Date("");
    }

    ToolButton {
        symbol: d.validDate ? Icons.faCalendarAlt : Icons.faCalendar
        font.family: Fonts.icons
        background: Item {}
        onClicked: {
            dueDateSelectionDialog.selectedDate = root.item.dueTo;
            dueDateSelectionDialog.open();
        }

        DateSelectionDialog {
            id: dueDateSelectionDialog
            onAccepted: root.item.dueTo = selectedDate
        }
    }

    Label {
        text: {
            let result = "";
            if (root.item.isRecurring) {
                result = qsTr("First due on %1.").arg(root.item.dueTo.toLocaleDateString());
            }
            return result;
        }
        Layout.columnSpan: 3
        Layout.fillWidth: true
        wrapMode: "WrapAtWordBoundaryOrAnywhere"
    }

    Label {
        text: {
            switch (root.item.recurrencePattern) {
            case OTL.ComplexItem.NoRecurrence:
                return qsTr("No recurrence pattern set...")
            case OTL.ComplexItem.RecurDaily:
                return qsTr("Recurs every day.")
            case OTL.ComplexItem.RecurWeekly:
                return qsTr("Recurs every week.")
            case OTL.ComplexItem.RecurMonthly:
                return qsTr("Recurs every month.")
            case OTL.ComplexItem.RecurEveryNDays:
                return qsTr("Recurs every %1 days.").arg(root.item.recurInterval);
            default:
                console.warn(
                            "Warning: Unhandled recurrence pattern",
                            root.item.recurrencePattern);
                return "";
            }
        }
        Layout.columnSpan: 2
        Layout.fillWidth: true
        wrapMode: "WrapAtWordBoundaryOrAnywhere"
    }

    ToolButton {
        symbol: Icons.faSync
        font.family: Fonts.solidIcons
        background: Item {}
        onClicked: recurrenceDialog.edit(root.item)

        RecurrenceDialog { id: recurrenceDialog }
    }

}
