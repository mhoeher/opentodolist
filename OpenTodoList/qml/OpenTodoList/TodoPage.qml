/*
 *  OpenTodoList - A todo and task manager
 *  Copyright (C) 2014  Martin Höher <martin@rpdev.net>
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
import QtQuick.Layouts 1.1
import net.rpdev.OpenTodoList.Core 1.0
import net.rpdev.OpenTodoList.Theme 1.0
import net.rpdev.OpenTodoList.Components 1.0
import net.rpdev.OpenTodoList.Views 1.0

Page {
    id: page

    property Todo todo

    name: todo && todo.title !== "" ? todo.title : qsTr( "Untitled Todo" )

    TabView {
        anchors.fill: parent
        showActionButton: true
        actionButtonTitle: qsTr( "Todo Lists" )

        onActionButtonClicked: page.close()

        Tab {
            name: page.name

            Flickable {
                id: flick
                anchors.fill: parent
                anchors.margins: Measures.smallSpace
                contentWidth: width
                contentHeight: grid.height

                GridLayout {
                    id: grid
                    columns: 2
                    anchors {
                        left: parent.left
                        right: parent.right
                        top: parent.top
                    }

                    SymbolLabel {
                        id: titleLabel
                        symbol: page.todo && page.todo.isDone ? Symbols.checkedBox : Symbols.uncheckedBox
                        text: page.todo ? page.todo.title : ""
                        placeholder: qsTr( "Untitled Todo" )
                        useSymbolButton: true
                        Layout.columnSpan: 2
                        Layout.fillWidth: true
                        Layout.preferredHeight: height

                        onSymbolClicked: if ( page.todo ) page.todo.toggle()
                        onClicked: {
                            var dialog = editor.createObject( this );
                            dialog.show();
                        }

                        property Component editor: Component {
                            LineEditDialog {
                                inputLabel: qsTr( "Rename Todo:" )
                                text: todo.title
                                onTextAvailable: if ( page.todo ) page.todo.title = text
                            }
                        }
                    }

                    Label {
                        text: qsTr( "Progress:" )
                        font.bold: true
                    }
                    Slider {
                        minimum: 0
                        maximum: 100
                        value: page.todo ? page.todo.progress : 0
                        Layout.fillWidth: true
                        Layout.maximumWidth: Measures.minimumPageWidth

                        onValueChanged: if ( page.todo ) todo.progress = value
                    }

                    Label {
                        text: qsTr( "Priority:" )
                        font.bold: true
                    }
                    Slider {
                        minimum: -1
                        maximum: 10
                        value: page.todo ? page.todo.priority : -1
                        Layout.fillWidth: true
                        Layout.maximumWidth: Measures.minimumPageWidth
                        color: Colors.colorForPriority( page.todo ? todo.priority : -1 )

                        onValueChanged: if ( page.todo ) todo.priority = value;
                    }

                    SymbolLabel {
                        Layout.fillWidth: true
                        Layout.columnSpan: 2
                        text: page.todo ? Qt.formatDate( page.todo.dueDate ) : ""
                        placeholder: qsTr( "No due date selected" )
                        symbol: text === "" ? Symbols.calendarEmpty : Symbols.calendarFull

                        property Component datePickerDialog: DateSelectionDialog {
                            onDateSelected: if ( page.todo ) page.todo.dueDate = selectedDate
                            onDateCleared:  if ( page.todo ) page.todo.dueDate = new Date( NaN, NaN, NaN );
                        }

                        onClicked: {
                            var dialog = datePickerDialog.createObject( this );
                            var currentDate = page.todo ? page.todo.dueDate : new Date();
                            if ( isNaN( currentDate.getFullYear() ) || isNaN( currentDate.getMonth() ) ||
                                    isNaN( currentDate.getDate() ) ) {
                                currentDate = new Date();
                            }
                            dialog.focusDate = currentDate;
                            if ( page.todo ) {
                                dialog.highlightDates = [ page.todo.dueDate ];
                            }
                            dialog.show();
                        }
                    }

                    Item {
                        Layout.columnSpan: 2
                        Layout.preferredHeight: notesBackground.height + 2 * Measures.smallSpace
                        Layout.fillWidth: true

                        Rectangle {
                            id: notesBackground

                            gradient: Gradient {
                                GradientStop {
                                    position: 0.0
                                    color: Qt.tint( Colors.primary, "#ddffffff" )
                                }
                                GradientStop {
                                    position: 1.0
                                    color: Qt.tint( Colors.primary, "#bbffffff" )
                                }
                            }
                            border {
                                color: Qt.tint( Colors.primary, "#ccffffff" )
                                width: Measures.smallBorderWidth
                            }
                            anchors {
                                left: parent.left
                                right: parent.right
                                top: parent.top
                                margins: Measures.smallSpace
                            }
                            height: notesLabel.height + 2 * Measures.tinySpace

                            Label {
                                id: notesLabel

                                text: page.todo ? page.todo.description : ""
                                anchors {
                                    left: parent.left
                                    right: parent.right
                                    top: parent.top
                                    margins: Measures.tinySpace
                                }
                                textFormat: Text.RichText
                                wrapMode: Text.WrapAtWordBoundaryOrAnywhere

                                MouseArea {
                                    anchors.fill: parent

                                    property Component dialog: RichTextEditDialog {
                                        text: page.todo ? page.todo.description : ""
                                        onAccept: if ( page.todo ) page.todo.description = text
                                    }

                                    onClicked: {
                                        var d = dialog.createObject( this );
                                        d.show();
                                    }
                                }
                            }
                        }
                    }
                    TodoView {
                        id: subTodosView

                        Layout.columnSpan: 2
                        Layout.fillWidth: true
                        Layout.preferredHeight: 200
                        todos: TodoModel {
                            parentTodo: page.todo
                            library: page.todo ? page.todo.library : null
                            queryType: TodoModel.QuerySubTodosOfTodo

                            onParentTodoChanged: update()
                        }
                    }
                }
            }
            ScrollBar {
                target: flick
            }
        }
    }
}
