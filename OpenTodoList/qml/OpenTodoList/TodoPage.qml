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

import QtQuick 2.2
import QtQuick.Layouts 1.1
import net.rpdev.OpenTodoList.DataModel 1.0
import net.rpdev.OpenTodoList.Models 1.0
import net.rpdev.OpenTodoList.SystemIntegration 1.0
import net.rpdev.OpenTodoList.Theme 1.0
import net.rpdev.OpenTodoList.Components 1.0
import net.rpdev.OpenTodoList.Views 1.0

Page {
    id: page

    property Todo todo: Todo {}

    name: todo.title !== "" ? todo.title : qsTr( "Untitled Todo" )

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
                        symbol: page.todo.isDone ? Symbols.checkedBox : Symbols.uncheckedBox
                        text: page.todo.title
                        placeholder: qsTr( "Untitled Todo" )
                        useSymbolButton: true
                        Layout.columnSpan: 2
                        Layout.fillWidth: true
                        Layout.preferredHeight: height

                        onSymbolClicked: page.todo.toggle()
                        onClicked: {
                            var dialog = editor.createObject( this );
                            dialog.show();
                        }

                        property Component editor: Component {
                            LineEditDialog {
                                inputLabel: qsTr( "Rename Todo:" )
                                text: todo.title
                                onTextAvailable: page.todo.title = text
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
                        value: page.todo.progress
                        Layout.fillWidth: true
                        Layout.maximumWidth: Measures.minimumPageWidth

                        onValueChanged: todo.progress = value
                    }

                    Label {
                        text: qsTr( "Priority:" )
                        font.bold: true
                    }
                    Slider {
                        minimum: -1
                        maximum: 10
                        value: page.todo.priority
                        Layout.fillWidth: true
                        Layout.maximumWidth: Measures.minimumPageWidth
                        color: Colors.colorForPriority( todo.priority )

                        onValueChanged: todo.priority = value;
                    }

                    SymbolLabel {
                        Layout.fillWidth: true
                        Layout.columnSpan: 2
                        text: Qt.formatDate( page.todo.dueDate )
                        placeholder: qsTr( "No due date selected" )
                        symbol: text === "" ? Symbols.calendarEmpty : Symbols.calendarFull

                        property Component datePickerDialog: DateSelectionDialog {
                            onDateSelected: page.todo.dueDate = selectedDate
                            onDateCleared:  page.todo.dueDate = new Date( NaN, NaN, NaN );
                        }

                        onClicked: {
                            var dialog = datePickerDialog.createObject( this );
                            var currentDate = page.todo.dueDate
                            if ( isNaN( currentDate.getFullYear() ) || isNaN( currentDate.getMonth() ) ||
                                    isNaN( currentDate.getDate() ) ) {
                                currentDate = new Date();
                            }
                            dialog.focusDate = currentDate;
                            dialog.highlightDates = [ page.todo.dueDate ];
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
                                topMargin: Measures.smallSpace
                            }
                            height: notesLabel.height + 2 * Measures.tinySpace

                            Label {
                                id: notesLabel

                                text: page.todo.description
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
                                        onAccept: page.todo.description = this.text;
                                    }

                                    onClicked: {
                                        var d = dialog.createObject( this );
                                        d.text = page.todo.description;
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
                        Layout.preferredHeight: contentItem.height + headerHeight
                        backgroundVisible: true
                        todos: TodoModel {
                            id: subTodosModel
                            queryType: TodoModel.QuerySubTodosOfTodo
                            parentTodo: page.todo
                            sortMode: root.sortMode
                        }
                        clip: true
                        interactive: false

                        onTodoSelected: {
                            var component = Qt.createComponent( "TodoPage.qml" );
                            if ( component.status === Component.Ready ) {
                                var newPage = component.createObject( page.parent );
                                newPage.todo.shadow( todo );
                            } else {
                                console.error( component.errorString() );
                            }
                        }
                    }
                }
            }
            ScrollBar {
                target: flick
            }
        }
    }

    resources: [
        Shortcut {
            keySequence: fromStandardKey( StandardKey.Close )
            onTriggered: page.close()
            enabled: page.isTopmostPage
        },
        Shortcut {
            keySequence: qsTr( "Ctrl++")
            onTriggered: if ( todo ) todo.progress = todo.progress + 10
            enabled: page.isTopmostPage
        },
        Shortcut {
            keySequence: qsTr( "Ctrl+-")
            onTriggered: if ( todo ) todo.progress = todo.progress - 10
            enabled: page.isTopmostPage
        },
        Shortcut {
            keySequence: qsTr( "Ctrl+Up")
            onTriggered: if ( todo ) todo.priority = todo.priority + 1
            enabled: page.isTopmostPage
        },
        Shortcut {
            keySequence: qsTr( "Ctrl+Down")
            onTriggered: if ( todo ) todo.priority = todo.priority - 1
            enabled: page.isTopmostPage
        },
        Shortcut {
            keySequence: qsTr( "Ctrl+Left")
            onTriggered: {
                if ( todo ) {
                    var date = todo.dueDate;
                    if ( !isNaN(date.getDate()) ) {
                        date.setDate( date.getDate() - 1 );
                        todo.dueDate = date;
                    } else {
                        todo.dueDate = new Date();
                    }
                }
            }
            enabled: page.isTopmostPage
        },
        Shortcut {
            keySequence: qsTr( "Ctrl+Right")
            onTriggered: {
                if ( todo ) {
                    var date = todo.dueDate;
                    if ( !isNaN(date.getDate()) ) {
                        date.setDate( date.getDate() + 1 );
                        todo.dueDate = date;
                    } else {
                        todo.dueDate = new Date();
                    }
                }
            }
            enabled: page.isTopmostPage
        }
    ]
}
