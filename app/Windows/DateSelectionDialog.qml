import QtQuick 2.9
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12
import Qt.labs.calendar 1.0

import "../Components"
import "../Fonts"
import "../Utils"

CenteredDialog {
    id: dialog

    property date selectedDate

    standardButtons: Dialog.Ok | Dialog.Cancel | Dialog.Reset
    onReset: dialog.selectedDate = new Date("")
    onSelectedDateChanged: {
        if (selectedDate.getTime() === selectedDate.getTime()) {
            // Selected date is valid
            grid.year = selectedDate.getFullYear();
            grid.month = selectedDate.getMonth();
        }
    }
    header: ToolBar {
        RowLayout {
            anchors.horizontalCenter: parent.horizontalCenter
            ToolButton {
                symbol: Icons.faAngleDoubleLeft
                onClicked: grid.year -= 1
            }

            ToolButton {
                symbol: Icons.faAngleLeft
                onClicked: {
                    var month = grid.month;
                    if (month === 0) {
                        grid.year -= 1;
                        grid.month = 11;
                    } else {
                        grid.month -= 1;
                    }
                }
            }

            Label {
                Layout.fillWidth: true
                horizontalAlignment: Text.AlignHCenter
                text: grid.locale.monthName(grid.month) + " " + grid.year
            }

            ToolButton {
                symbol: Icons.faAngleRight
                onClicked: {
                    var month = grid.month;
                    if (month === 11) {
                        grid.year += 1;
                        grid.month = 0;
                    } else {
                        grid.month += 1;
                    }
                }
            }

            ToolButton {
                symbol: Icons.faAngleDoubleRight
                onClicked: grid.year += 1
            }
        }
    }

    QtObject {
        id: d

        function dateEquals(date1, date2) {
            return (date1) && (date2) &&
                   (date1.getFullYear() === date2.getFullYear()) &&
                   (date1.getMonth() === date2.getMonth()) &&
                   (date1.getDate() === date2.getDate());
        }
    }

    GridLayout {
        columns: 2

        Item { width: 1; height: 1 }

        DayOfWeekRow {
            id: dayOfWeekRow
            locale: grid.locale
            Layout.fillWidth: true
        }

        WeekNumberColumn {
            id: weekNumbers
            month: grid.month
            year: grid.year
            locale: grid.locale
            Layout.fillHeight: true
        }

        FontMetrics {
            id: fontMetrics
        }

        MonthGrid {
            id: grid

            delegate: ToolButton {
                opacity: model.month === grid.month ? 1.0 : 0.5
                text: model.day
                font.pixelSize: grid.font.pixelSize
                checked: d.dateEquals(model.date, dialog.selectedDate)
                background: Pane {
                    Material.background: Material.Green
                    opacity: 0.5
                    visible: checked
                }
                onClicked: dialog.selectedDate = date
            }
        }
    }
}
