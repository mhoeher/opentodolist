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
import net.rpdev.OpenTodoList 1.0
import "Utils.js" as Utils

View {
    id: todoDetailsView
    hidden: todo ? false : true
    
    property QtObject todo: null
    property TodoSortFilterModel model: TodoSortFilterModel {
        sourceModel: todo ? todo.subTodos : null
        searchString: filterText.text
    }

    onTodoChanged: {
        filterText.text = "";
        if ( todo ) {
            todoDueDateEdit.date = todo.dueDate;
        } else {
            todoDueDateEdit.date = new Date( NaN, NaN, NaN );
        }
    }
    
    toolButtons: [
        ToolButton {
            font.family: symbolFont.name
            label: "\uf060"

            onClicked: {
                todoDetailsView.todo = todoDetailsView.todo.parentTodo
            }
        }
    ]

    Flickable {
        anchors.fill: parent
        contentWidth: detailsContents.width
        contentHeight: detailsContents.height

        Item {
            id: detailsContents
            width: parent.width
            height: childrenRect.height

            Column {
                width: todoDetailsView.clientWidth
                spacing: 10

                Item {
                    width: parent.width
                    height: childrenRect.height

                    TextInput {
                        id: todoDetailsViewTitleEdit
                        text: todoDetailsView.todo ? todoDetailsView.todo.title : ""
                        font.bold: true
                        font.pointSize: 16
                        width: parent.width - saveTitleButton.width
                        wrapMode: Text.Wrap

                        function saveTitle() {
                            todoDetailsView.todo.title = todoDetailsViewTitleEdit.text;
                            todoDetailsViewTitleEdit.focus = false;
                        }

                        Keys.onEnterPressed: saveTitle()
                        Keys.onReturnPressed: saveTitle()
                    }

                    Button {
                        id: saveTitleButton
                        label: "Save"
                        onClicked: todoDetailsViewTitleEdit.saveTitle()
                        visible: todoDetailsViewTitleEdit.focus
                        anchors.right: parent.right
                    }
                }

                Flow {
                    spacing: 10
                    width: parent.width
                    ProgressIndicator {
                        id: progressIndicator
                        percentage: todoDetailsView.todo ? todoDetailsView.todo.progress : 0

                        onDecreasePressed: todoDetailsView.todo.progress = stepDown()
                        onIncreasePressed: todoDetailsView.todo.progress = stepUp()
                    }
                    PriorityIndicator {
                        width: progressIndicator.width
                        height: progressIndicator.height
                        priority: todoDetailsView.todo ? todoDetailsView.todo.priority : -1

                        onSelectedPriority: todoDetailsView.todo.priority = priority
                    }
                    DatePicker {
                        id: todoDueDateEdit
                        baseWidth: progressIndicator.width
                        baseHeight: progressIndicator.height
                        date: todoDetailsView.todo ? todoDetailsView.todo.dueDate : Utils.getNullDate()

                        onDateChanged: {
                            if ( todoDetailsView.todo ) {
                                todoDetailsView.todo.dueDate = date
                            }
                        }
                    }
                }

                Row {
                    width: parent.width

                    SimpleTextInput {
                        id: newSubTodoTitle
                        text: ""
                        placeholderText: "Add new subtodo"
                        width: parent.width - addNewSubTodoButton.width

                        onApply: addNewSubTodoButton.createNewSubTodo()
                    }

                    Button {
                        id: addNewSubTodoButton
                        label: "\uf067"
                        font.family: symbolFont.name
                        onClicked: createNewSubTodo()

                        function createNewSubTodo() {
                            var todo = todoDetailsView.todo.todoList.addTodo();
                            todo.title = newSubTodoTitle.text;
                            todo.parentTodo = todoDetailsView.todo;
                            newSubTodoTitle.text = "";
                        }
                    }
                }

                SimpleTextInput {
                    id: filterText
                    placeholderText: "Filter subtodos"
                    anchors.left: parent.left
                    anchors.right: parent.right
                }

                TodoView {
                    autoSize: true
                    width: parent.width
                    model: todoDetailsView.model
                    onTodoSelected: todoDetailsView.todo = todo
                }

                Item {
                    width: parent.width
                    height: childrenRect.height

                    BorderImage {
                        width: todoDescriptionEditContains.width
                        height: todoDescriptionEditContains.height
                        source: "description.sci"
                        cache: !settings.debug
                    }

                    MouseArea {
                        width: todoDescriptionEditContains.width
                        height: todoDescriptionEditContains.height
                        onClicked: todoDescriptionEdit.focus = true;
                    }

                    Item {
                        id: todoDescriptionEditContains
                        width: parent.width
                        height: childrenRect.height + 40

                        TextEdit {
                            id: todoDescriptionEdit

                            text: todoDetailsView.todo ? todoDetailsView.todo.description : ""
                            textFormat: TextEdit.RichText
                            wrapMode: Text.Wrap
                            width: parent.width - 40
                            x: 20
                            y: 20
                        }
                    }
                }

                Button {
                    label: "Save"
                    anchors.right: parent.right

                    onClicked: { todoDetailsView.todo.description = todoDescriptionEdit.text; }
                }
            }
        }
    }
}
