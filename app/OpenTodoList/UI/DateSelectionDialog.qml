import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import Qt.labs.calendar 1.0

import OpenTodoList.UI 1.0

CenteredDialog {
    id: dialog

    property date selectedDate

    width: parent.width * 0.8
    height: parent.height * 0.8
    standardButtons: Dialog.Ok | Dialog.Cancel
    onSelectedDateChanged: {
        console.debug(selectedDate);
        if (selectedDate.getTime() === selectedDate.getTime()) {
            // Selected date is valid
            grid.year = selectedDate.getFullYear();
            grid.month = selectedDate.getMonth();
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
        columns: 3
        width: Math.min(dialog.availableWidth, dialog.availableHeight) - Globals.defaultMargin
        height: width
        anchors.centerIn: parent

        Symbol {
            symbol: Fonts.symbols.faAngleLeft
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
            wrapMode: Text.Word
        }

        Symbol {
            symbol: Fonts.symbols.faAngleRight
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

        DayOfWeekRow {
            locale: grid.locale

            Layout.row: 1
            Layout.column: 1
            Layout.fillWidth: true
        }

        WeekNumberColumn {
            month: grid.month
            year: grid.year
            locale: grid.locale

            Layout.row: 2
            Layout.fillHeight: true
        }

        MonthGrid {
            id: grid

            Layout.row: 2
            Layout.column: 1
            Layout.fillWidth: true
            Layout.fillHeight: true
            delegate: Label {
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                color: {
                    if (d.dateEquals(model.date, dialog.selectedDate)) {
                        return Colors.highlightedText;
                    } else {
                        return Colors.windowText;
                    }
                }
                opacity: model.month === grid.month ? 1.0 : 0.5
                text: model.day
                font.pixelSize: grid.font.pixelSize
                background: Rectangle {
                    color: {
                        if (d.dateEquals(model.date, dialog.selectedDate)) {
                            return Colors.highlight;
                        } else if (model.month === grid.month) {
                            return Colors.light;
                        } else {
                            return Colors.midlight;
                        }
                    }

                }
            }
            onClicked: dialog.selectedDate = date
        }

        Label {
            Layout.row: 3
            Layout.column: 1
            text: dialog.selectedDate.toLocaleDateString()
            wrapMode: Text.Word
        }

        Symbol {
            Layout.row: 3
            Layout.column: 2
            symbol: Fonts.symbols.faTimes
            onClicked: dialog.selectedDate = new Date("")

            // https://stackoverflow.com/a/12372720/6367098
            visible: dialog.selectedDate.getTime() === dialog.selectedDate.getTime()
        }

    }
}
