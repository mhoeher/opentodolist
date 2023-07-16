import QtQuick 2.9
import QtQuick.Layouts 1.3

import OpenTodoList 1.0 as OTL

import "../Components"
import "../Controls" as C
import "../Fonts"
import "../Utils"
import "../Windows"

Column {
    id: root

    property OTL.ComplexItem item: null

    visible: DateUtils.validDate(item.dueTo)

    QtObject {
        id: d

        property bool validDate: DateUtils.validDate(root.item.dueTo)
    }

    C.ItemDelegate {
        id: heading

        width: parent.width

        contentItem: Heading {
            level: 2
            text: qsTr("Due on") + " " + root.item.effectiveDueTo.toLocaleDateString()
            wrapMode: "WrapAtWordBoundaryOrAnywhere"
            Layout.fillWidth: true

            rightPadding: setDueDateToolButton.width + AppSettings.mediumSpace
            verticalAlignment: Text.AlignVCenter

            C.ToolButton {
                id: setDueDateToolButton
                symbol: d.validDate ? Icons.mdiEvent : Icons.mdiCalendarToday
                background: Item {}
                anchors.right: parent.right
                anchors.verticalCenter: parent.verticalCenter
                onClicked: heading.clicked()
            }
        }

        onClicked: mouse => {
                       dueDateSelectionDialog.selectedDate = root.item.dueTo
                       dueDateSelectionDialog.open()
                   }

        DateSelectionDialog {
            id: dueDateSelectionDialog
            onAccepted: root.item.dueTo = selectedDate
        }
    }

    C.ItemDelegate {
        id: firstDueItem

        width: parent.width
        visible: root.item.isRecurring
        enabled: false

        contentItem: C.Label {
            text: {
                let result = ""
                if (root.item.isRecurring) {
                    result = qsTr("First due on %1.").arg(
                                root.item.dueTo.toLocaleDateString())
                }
                return result
            }

            verticalAlignment: Text.AlignVCenter
        }
    }

    C.ItemDelegate {
        id: recurrenceModeItem

        width: parent.width

        contentItem: C.Label {
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
                case OTL.ComplexItem.RecurYearly:
                    return qsTr("Recurs every year.")
                case OTL.ComplexItem.RecurEveryNDays:
                    return qsTr("Recurs every %1 days.").arg(
                                root.item.recurInterval)
                case OTL.ComplexItem.RecurEveryNWeeks:
                    return qsTr("Recurs every %1 weeks.").arg(
                                root.item.recurInterval)
                case OTL.ComplexItem.RecurEveryNMonths:
                    return qsTr("Recurs every %1 months.").arg(
                                root.item.recurInterval)
                default:
                    console.warn("Warning: Unhandled recurrence pattern",
                                 root.item.recurrencePattern)
                    return ""
                }
            }

            rightPadding: setRecurrenceModeToolButton.width + AppSettings.mediumSpace
            verticalAlignment: Text.AlignVCenter

            C.ToolButton {
                id: setRecurrenceModeToolButton
                symbol: Icons.mdiRepeat
                background: Item {}
                anchors.right: parent.right
                anchors.verticalCenter: parent.verticalCenter
                onClicked: recurrenceModeItem.clicked()
            }
        }

        onClicked: mouse => recurrenceDialog.edit(root.item)

        RecurrenceDialog {
            id: recurrenceDialog
        }
    }

    C.ItemDelegate {
        id: recurUntilItem

        width: parent.width
        visible: root.item.isRecurring

        contentItem: C.Label {
            text: {
                if (DateUtils.validDate(root.item.recurUntil)) {
                    return qsTr("Recurs until %1.").arg(
                                root.item.recurUntil.toLocaleDateString())
                } else {
                    return qsTr("Recurs indefinitely")
                }
            }
            rightPadding: setRecurUntilToolButton.width + AppSettings.mediumSpace
            verticalAlignment: Text.AlignVCenter

            C.ToolButton {
                id: setRecurUntilToolButton
                symbol: Icons.mdiCalendarToday
                background: Item {}
                anchors.right: parent.right
                anchors.verticalCenter: parent.verticalCenter
                onClicked: recurUntilItem.clicked()
            }
        }
        onClicked: mouse => {
                       recurUntilSelectionDialog.selectedDate = root.item.recurUntil
                       recurUntilSelectionDialog.open()
                   }

        DateSelectionDialog {
            id: recurUntilSelectionDialog
            onAccepted: {
                root.item.recurUntil = recurUntilSelectionDialog.selectedDate
            }
        }
    }
}
