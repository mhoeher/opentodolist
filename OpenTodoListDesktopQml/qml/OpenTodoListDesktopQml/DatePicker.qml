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
    id: root

    height: childrenRect.height
    width: childrenRect.width

    property date date: new Date()
    property string label: "Selected Date"
    property string nullDateLabel: "No date set"

    Column {

        Row {
            spacing: 10
            Text {
                text: root.label
            }
            Text {
                text: Utils.formatDate( root.date, root.nullDateLabel )
            }
            Button {
                label: "Select Date"
                visible: root.state == ""
                onClicked: {
                    calendar.viewDate = Utils.isValidDate( root.date ) ? root.date : new Date()
                    calendar.selectedDate = root.date
                    root.state = "selecting"
                }
            }
            Button {
                label: "OK"
                visible: root.state == "selecting"
                onClicked: {
                    root.date = calendar.selectedDate
                    root.state = ""
                }
            }
            Button {
                label: "Clear"
                visible: root.state == "selecting"
                onClicked: {
                    root.date = Utils.getNullDate()
                    root.state = ""
                }
            }

            Button {
                label: "Cancel"
                visible: root.state == "selecting"
                onClicked: {
                    calendar.selectedDate = root.date
                    root.state = ""
                }
            }
        }

        Row {
            visible: root.state == "selecting"
            Calendar {
                id: calendar
                viewDate: Utils.isValidDate( root.date ) ? root.date : new Date()
                selectedDate: root.date
            }
        }
    }
}
