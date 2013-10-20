/*
 *  OpenTodoList - A todo and task manager
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
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0

import "../../../js/Utils.js" as Utils

Item {
    id: edit

    property var date: Utils.getNullDate()

    width: childrenRect.width
    height: childrenRect.height

    function clear() {
        edit.date = Utils.getNullDate();
    }

    function applySelectedDate( date ) {
        edit.date = date;
    }

    function select() {
        var component = Qt.createComponent( "../dialogs/DatePickerDialog.qml" );
        var dialog = component.createObject( window );
        dialog.dateSelected.connect( applySelectedDate );
        dialog.exec( date );
    }

    RowLayout {
        Label {
            id: dateLabel
            text: Utils.formatDate( edit.date, qsTr( "No date set" ) )
        }
        Button {
            text: qsTr( "Select" );
            onClicked: edit.select()
        }
        Button {
            text: qsTr( "Clear" )
            onClicked: edit.clear()
        }
    }
}
