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

import "../components"
import "../../../js/Utils.js" as Utils

GroupBox {
    id: view

    property QtObject todo: null

    function showTodo() {
        nameEdit.text = todo.title;
        descriptionEdit.text = todo.description;
        date.date = todo.dueDate;
        priority.currentIndex = todo.priority + 1;
        progressEdit.value = todo.progress;
        todo.onProgressChanged.connect( updateProgress );
    }

    function updateProgress() {
        if ( todo ) {
            progressEdit.progress = todo.progress;
        }
    }

    onTodoChanged: {
        if ( todo ) {
            showTodo();
        }
    }

    GridLayout {
        columns: 2
        anchors.fill: parent

        Label {
            text: qsTr( "Title:" )
        }

        TextField {
            id: nameEdit
            placeholderText: qsTr( "Enter a title for the todo here" )
            Layout.fillWidth: true
        }

        Label {
            text: qsTr( "Due to:" )
        }

        DateEdit {
            id: date
        }

        Label {
            text: qsTr( "Priority:" )
        }

        ComboBox {
            id: priority
            model: [
                qsTr( "No Priority" ),
                qsTr( "0 - Lowest" ),
                qsTr( "1" ),
                qsTr( "2" ),
                qsTr( "3" ),
                qsTr( "4" ),
                qsTr( "5" ),
                qsTr( "6" ),
                qsTr( "7" ),
                qsTr( "8" ),
                qsTr( "9" ),
                qsTr( "10 - Highest" ) ]
        }

        Label {
            text: qsTr( "Progress:" )
        }

        Slider {
            id: progressEdit
            minimumValue: 0
            maximumValue: 100
            stepSize: 10
        }

        Label {
            text: qsTr( "Description:" )
        }

        TextArea {
            id: descriptionEdit
            text: ""
            textFormat: TextEdit.RichText

            Layout.fillWidth: true
            Layout.fillHeight: true
        }

        Item {
            Layout.columnSpan: 2
            Layout.fillWidth: true
            height: childrenRect.height
            Row {
                anchors.right: parent.right

                Button {
                    text: qsTr( "Apply" )

                    onClicked: {
                        todo.title = nameEdit.text;
                        todo.description = descriptionEdit.text;
                        todo.progress = progressEdit.value;
                        todo.priority = priority.currentIndex - 1;
                    }
                }

                Button {
                    text: qsTr( "Reset" )

                    onClicked: {
                        view.showTodo();
                    }
                }
            }
        }
    }
}
