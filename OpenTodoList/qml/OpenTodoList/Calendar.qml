/*
 *  OpenTodoListDesktopQml - Desktop QML frontend for OpenTodoList
 *  Copyright (C) 2013  Martin Höher <martin@rpdev.net>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

import QtQuick 2.0
import "Utils.js" as Utils


Item {
    id: calendar

    width: childrenRect.width + spacing * 2
    height: childrenRect.height + spacing * 2

    property date viewDate: new Date()
    property date selectedDate: Utils.getNullDate()
    property int firstDay: Utils.getFirstDay( viewDate )
    property int daysInMonth: Utils.getDaysInMonth( viewDate )
    property int spacing: 10

    Column {
        x: calendar.spacing
        y: calendar.spacing
        width:  childrenRect.width
        height: childrenRect.height
        spacing: calendar.spacing

        Item {
            height: childrenRect.height
            width: calendarGrid.width
            Button {
                id: prevYearButton
                label: "<<"
                onClicked: calendar.viewDate =
                           new Date( calendar.viewDate.getFullYear() - 1,
                                     calendar.viewDate.getMonth(), 1 )
            }
            Button {
                id: prevMonthButton
                anchors.left: prevYearButton.right
                anchors.leftMargin: 10
                label: "<"
                onClicked: calendar.viewDate =
                           new Date( calendar.viewDate.getFullYear(),
                                    calendar.viewDate.getMonth() - 1, 1 )
            }
            Text {
                anchors.horizontalCenter: parent.horizontalCenter
                text: Utils.MonthNames[ calendar.viewDate.getMonth() ] + " " +
                      calendar.viewDate.getFullYear()
            }

            Button {
                id: nextMonthButton
                anchors.right: nextYearButton.left
                anchors.rightMargin: 10
                label: ">"
                onClicked: calendar.viewDate =
                           new Date( calendar.viewDate.getFullYear(),
                                    calendar.viewDate.getMonth() + 1, 1 )
            }
            Button {
                id: nextYearButton
                anchors.right: parent.right
                label: ">>"
                onClicked: calendar.viewDate =
                           new Date( calendar.viewDate.getFullYear() + 1,
                                     calendar.viewDate.getMonth(), 1 )
            }
        }

        Grid {
            id: calendarGrid
            width:  childrenRect.width
            height: childrenRect.height

            columns: 7
            spacing: calendar.spacing

            Repeater {
                model: 7
                Text {
                    text: Utils.DayNamesShort[ index ]
                }
            }
            Repeater {
                model: calendar.daysInMonth + calendar.firstDay
                Item {
                    id: cell
                    property bool isValid: index >= calendar.firstDay
                    opacity: isValid ? 1.0 : 0.0
                    width: childrenRect.width
                    height: childrenRect.height

                    Button {
                        label: index - calendar.firstDay + 1
                        down: Utils.areDatesEqual( thisDate, calendar.selectedDate )
                        property date thisDate: new Date(
                                                    calendar.viewDate.getFullYear(),
                                                    calendar.viewDate.getMonth(),
                                                    index - calendar.firstDay + 1 )
                        onClicked: if ( cell.isValid ) {
                                       calendar.selectedDate = thisDate;
                                   }
                    }
                }
            }
        }

    }

    Behavior on width { SmoothedAnimation { duration: 500 } }
    Behavior on height { SmoothedAnimation { duration: 500 } }
}
