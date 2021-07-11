import QtQuick 2.0
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.0

import OpenTodoList 1.0 as OTL

import "../Components" as Components


CenteredDialog {
    id: dialog

    property OTL.ComplexItem item: null

    property int recurrencePattern: item ? item.recurrencePattern : OTL.ComplexItem.NoRecurrence
    property int recurrenceSchedule: item ? item.recurrenceSchedule : OTL.ComplexItem.RelativeToOriginalDueDate
    property int recurInterval: item ? item.recurInterval : 1

    function edit(item) {
        dialog.item = item;
        recurrencePattern = item.recurrencePattern;
        recurrenceSchedule = item.recurrenceSchedule;
        recurInterval = item.recurInterval;
        d.updateRecurrencePatternEdit();
        open();
    }

    standardButtons: Dialog.Ok | Dialog.Cancel
    title: qsTr("Edit Recurrence")
    width: idealDialogWidth

    onAccepted: {
        if (dialog.item) {
            dialog.item.recurrencePattern = recurrencePattern;
            dialog.item.recurrenceSchedule = recurrenceSchedule;
            dialog.item.recurInterval = recurInterval;
        }
    }

    Component.onCompleted: {
        patternModel.dynamicRoles = true;
        patternModel.append({"name": qsTr("Never"), type: OTL.ComplexItem.NoRecurrence});
        patternModel.append({"name": qsTr("Daily"), type: OTL.ComplexItem.RecurDaily});
        patternModel.append({"name": qsTr("Weekly"), type: OTL.ComplexItem.RecurWeekly});
        patternModel.append({"name": qsTr("Monthly"), type: OTL.ComplexItem.RecurMonthly});
        patternModel.append({"name": qsTr("Yearly"), type: OTL.ComplexItem.RecurYearly});
        patternModel.append({"name": qsTr("Every N Days"), type: OTL.ComplexItem.RecurEveryNDays});
        patternModel.append({"name": qsTr("Every N Weeks"), type: OTL.ComplexItem.RecurEveryNWeeks});
        patternModel.append({"name": qsTr("Every N Months"), type: OTL.ComplexItem.RecurEveryNMonths});
        d.updateRecurrencePatternEdit();
    }

    QtObject {
        id: d

        function updateRecurrencePatternEdit() {
            recurrencePatternEdit.currentIndex = 0;
            for (let i = 0; i < patternModel.count; ++i) {
                var entry = patternModel.get(i);
                if (entry["type"] === recurrencePattern) {
                    recurrencePatternEdit.currentIndex = i;
                    break;
                }
            }
        }
    }


    GridLayout {
        width: dialog.availableWidth
        columns: 2

        Label {
            text: qsTr("Recurs:")
        }

        ComboBox {
            id: recurrencePatternEdit
            model: ListModel { id: patternModel }
            textRole: "name"
            valueRole: "type"
            Layout.fillWidth: true
            onCurrentValueChanged: dialog.recurrencePattern = currentValue
        }

        Label {
            text: qsTr("Number of days:")
            visible: dialog.recurrencePattern === OTL.ComplexItem.RecurEveryNDays ||
                     dialog.recurrencePattern === OTL.ComplexItem.RecurEveryNWeeks ||
                     dialog.recurrencePattern === OTL.ComplexItem.RecurEveryNMonths
        }

        SpinBox {
            from: 1
            to: Math.pow(2, 31) - 1
            value: dialog.recurInterval
            Layout.fillWidth: true
            visible: dialog.recurrencePattern === OTL.ComplexItem.RecurEveryNDays ||
                     dialog.recurrencePattern === OTL.ComplexItem.RecurEveryNWeeks ||
                     dialog.recurrencePattern === OTL.ComplexItem.RecurEveryNMonths
            onValueChanged: dialog.recurInterval = value
        }

        CheckBox {
            text: qsTr("Recur relative to the date when marking as done")
            checked: dialog.recurrenceSchedule === OTL.ComplexItem.RelativeToCurrentDate
            Layout.fillWidth: true
            Layout.columnSpan: 2
            visible: dialog.recurrencePattern !== OTL.ComplexItem.NoRecurrence &&
                     dialog.recurrencePattern !== OTL.ComplexItem.RecurDaily
            onCheckedChanged: {
                if (checked) {
                    dialog.recurrenceSchedule = OTL.ComplexItem.RelativeToCurrentDate;
                } else {
                    dialog.recurrenceSchedule = OTL.ComplexItem.RelativeToOriginalDueDate;
                }
            }
        }
    }
}
