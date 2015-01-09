/*
 *  OpenTodoList - A todo and task manager
 *  Copyright (C) 2015  Martin Höher <martin@rpdev.net>
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
import QtQuick.Controls 1.3
import QtQuick.Layouts 1.1
import QtQuick.Dialogs 1.2
import "../components" as Components
import "../style" as Style
import "../views" as Views
import "../utils/DateUtils.js" as DateUtils
import net.rpdev.OpenTodoList.DataModel 1.0
import net.rpdev.OpenTodoList.DataBase 1.0
import net.rpdev.OpenTodoList.Models 1.0

Components.Page {
    id: todoPage

    property Todo todo: null

    title: todoProperties.title

    onTodoChanged: {
        dbConnection.registerTodo(todo, DatabaseConnection.KeepSynced)
    }

    QtObject {
        id: todoProperties

        property alias todo: todoPage.todo
        property string title: todo ? todo.title : qsTr("Unnamed Todo")
        property bool done: todo ? todo.done : false
        property int priority: todo ? todo.priority : -1
        property string formattedDueDate: todo ? Qt.formatDate( todo.dueDate ) : ""
        property string dueDate: formattedDueDate === "" ? qsTr( "No Due Date" ) : formattedDueDate
        property string description: todo ? todo.description : ""
    }

    DatabaseConnection {
        id: dbConnection
        database: application.database
    }

    Dialog {
        id: dateSelectionDialog
        title: qsTr( "Select Due Date" )
        standardButtons: StandardButton.Ok | StandardButton.Cancel | StandardButton.Reset
        onAccepted: todoPage.todo.dueDate = calendar.selectedDate
        onReset: todoPage.todo.dueDate = new Date(NaN,NaN,NaN);

        Calendar {
            id: calendar
        }
    }

    ColumnLayout {
        id: column
        anchors.fill: parent
        anchors.margins: Style.Measures.tinySpace
        Item { // Check state and title
            Layout.fillWidth: true
            height: Math.max( checkBox.height,
                             todoTitle.height,
                             todoTitleEdit.height) +
                    Style.Measures.midSpace
            Components.Symbol {
                id: checkBox
                symbol: todoProperties.done ?
                            Style.Symbols.checkedBox :
                            Style.Symbols.uncheckedBox
                anchors {
                    left: parent.left
                    verticalCenter: parent.verticalCenter
                    margins: Style.Measures.tinySpace
                }
                onClicked: {
                    todoPage.todo.done = !todoPage.todo.done;
                }
            }
            Style.P {
                id: todoTitle
                text: todoProperties.title
                visible: !todoTitleEdit.visible
                anchors {
                    left: checkBox.right
                    right: parent.right
                    verticalCenter: parent.verticalCenter
                    margins: Style.Measures.tinySpace
                }
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
            }
            MouseArea {
                anchors {
                    top: parent.top
                    bottom: parent.bottom
                    left: todoTitle.left
                    right: todoTitle.right
                }
                onClicked: todoTitleEdit.visible = true
            }
            TextField {
                id: todoTitleEdit
                visible: false
                anchors {
                    left: checkBox.right
                    right: parent.right
                    verticalCenter: parent.verticalCenter
                    margins: Style.Measures.tinySpace
                }
                validator: RegExpValidator {
                    regExp: /.+/
                }
                onVisibleChanged: {
                    text = todoProperties.title
                }
                onEditingFinished: {
                    visible = false;
                }
                onAccepted: {
                    todoPage.todo.title = todoTitleEdit.text;
                }
            }
        }

        GridLayout { // Due date, priority, ...
            id: grid
            columns: 2
            Layout.fillWidth: true

            Style.P {
                text: qsTr( "Priority:" )
            }
            Slider {
                Layout.fillWidth: true
                minimumValue: -1
                maximumValue: 10
                value: todoProperties.priority
                onValueChanged: {
                    if ( todoPage.todo ) {
                        todoPage.todo.priority = value;
                    }
                    value = Qt.binding( function() {
                        return todoProperties.priority;
                    });
                }
            }
            Style.P {
                text: qsTr( "Due Date:")
            }
            Item {
                Layout.fillWidth: true
                height: Math.max( dueDateLabel.height,
                                 Style.Measures.optButtonHeight )
                Style.P {
                    id: dueDateLabel
                    text: todoProperties.dueDate
                    anchors {
                        left: parent.left
                        right: parent.right
                        verticalCenter: parent.verticalCenter
                    }
                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                }
                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        if ( DateUtils.isValidDate(todoPage.todo.dueDate) ) {
                            calendar.selectedDate = todoPage.todo.dueDate;
                        } else {
                            calendar.selectedDate = new Date();
                        }
                        dateSelectionDialog.open()
                    }
                }
            }
        }
        TabView { // Description and tasks
            Layout.fillHeight: true
            Layout.fillWidth: true
            frameVisible: true
            Tab {
                title: "Tasks"
                ScrollView {
                    Views.TasksView {
                        todo: todoPage.todo
                    }
                }
            }
            Tab {
                title: qsTr( "Description" )
                Item {
                    anchors.fill: parent
                    Components.TextAreaToolBar {
                        id: editorToolbar
                        anchors {
                            left: parent.left
                            right: parent.right
                            top: parent.top
                        }
                        editor: descriptionEdit
                    }
                    TextArea {
                        id: descriptionEdit
                        property bool updatingTodo: false
                        anchors {
                            left: parent.left
                            right: parent.right
                            top: editorToolbar.bottom
                            bottom: parent.bottom
                        }
                        textFormat: TextEdit.RichText
                        Component.onCompleted: {
                            text = todoProperties.description;
                            todoProperties.descriptionChanged.connect( function() {
                                if ( !updatingTodo ) {
                                    descriptionEdit.text = todoProperties.description;
                                }
                            });
                            textChanged.connect( function() {
                                updatingTodo = true;
                                todoPage.todo.description = text;
                                updatingTodo = false;
                            });
                        }
                    }
                }
            }
        }
    }
}

