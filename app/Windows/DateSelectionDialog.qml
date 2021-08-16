import QtQuick 2.9
import QtQuick.Layouts 1.3
import QtQuick.Controls.Material 2.12
import Qt.labs.calendar 1.0

import "../Components"
import "../Controls" as C
import "../Fonts"
import "../Utils"

CenteredDialog {
    id: dialog

    property date selectedDate

    standardButtons: C.Dialog.Ok | C.Dialog.Cancel | C.Dialog.Reset
    onReset: dialog.selectedDate = new Date("")
    onSelectedDateChanged: {
        if (selectedDate.getTime() === selectedDate.getTime()) {
            // Selected date is valid
            grid.year = selectedDate.getFullYear();
            grid.month = selectedDate.getMonth();
        }
    }
    header: C.ToolBar {
        RowLayout {
            anchors.horizontalCenter: parent.horizontalCenter
            C.ToolButton {
                symbol: Icons.mdiFirstPage
                onClicked: grid.year -= 1
            }

            C.ToolButton {
                symbol: Icons.mdiKeyboardArrowLeft
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

            C.Label {
                Layout.fillWidth: true
                horizontalAlignment: Text.AlignHCenter
                text: grid.locale.monthName(grid.month) + " " + grid.year
            }

            C.ToolButton {
                symbol: Icons.mdiKeyboardArrowRight
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

            C.ToolButton {
                symbol: Icons.mdiLastPage
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

        Empty {}

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

        MonthGrid {
            id: grid

            delegate: C.ToolButton {
                property var localDate: new Date(year, month, day)
                property bool isToday: d.dateEquals(localDate, new Date()) // Highlight today

                opacity: model.month === grid.month ? 1.0 : 0.5
                checked: d.dateEquals(localDate, dialog.selectedDate)
                text: day
                background: C.Pane {
                    Material.background: {
                        if (checked) {
                            return Material.Green;
                        } else if (isToday) {
                            return Material.Purple;
                        } else {
                            return "white";
                        }
                    }
                    opacity: 0.5
                    visible: checked || isToday
                    enabled: false
                }
                contentItem: C.Label {
                    text: model.day
                    font.pixelSize: grid.font.pixelSize
                    font.bold: d.dateEquals(localDate, new Date()) // Make today bold
                    font.family: AppSettings.defaultFontFamily
                    wrapMode: "NoWrap"
                }

                onClicked: dialog.selectedDate = localDate
            }
        }
    }
}
