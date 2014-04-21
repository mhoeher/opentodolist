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
import net.rpdev.OpenTodoList.Core 1.0
import "../js/Utils.js" as Utils
import "../controls"

View {
    id: todoDetailsView
    hidden: todo ? false : true
    
    property QtObject todo: null
    property TodoModel model: null


    onTodoChanged: {
        if ( todo ) {
            todoDueDateEdit.date = todo.dueDate;
        } else {
            todoDueDateEdit.date = new Date( NaN, NaN, NaN );
        }
        priorityIndicator.focus = false
        // Ensure changes in description are saved
        saveDescriptionTimer.save();
        // Update the bread crump
        updateBreadCrump();
    }

    function updateBreadCrump() {
        var parents = [];
        if ( todoDetailsView.todo ) {
            var parent = todoDetailsView.todo.parentTodo;
            while ( parent ) {
                parents.unshift( parent );
                parent = parent.parentTodo;
            }
        }
        breadCrump.model = parents
    }
    
    toolButtons: [
        ToolButton {
            symbol: "\uf060"
            isBackButton: true

            onClicked: {
                todoDetailsView.todo = todoDetailsView.todo.parentTodo
            }
        }
    ]

    Flickable {
        anchors.fill: parent
        contentWidth: parent.width
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
                    Column {
                        width: parent.width
                        Repeater {
                            id: breadCrump
                            delegate: Item {
                                id: parentLink
                                width: todoDetailsView.clientWidth
                                height: childrenRect.height
                                Item {
                                    id: bcSpace
                                    height: 1
                                    width: index * 20
                                }
                                Text {
                                    id: bcSymbol
                                    anchors { left: bcSpace.right; leftMargin: 10 }
                                    font.family: symbolFont.name
                                    font.pointSize: fonts.h2
                                    text: "\uf101"
                                }
                                Text {
                                    anchors { left: bcSymbol.right; leftMargin: 10; right: parent.left }
                                    font.pointSize: fonts.h2
                                    text: modelData.title
                                }
                                MouseArea {
                                    anchors.fill: parentLink
                                    onClicked: todoDetailsView.todo = modelData
                                }
                            }
                        }
                    }
                    Behavior on height { NumberAnimation { duration: 300 } }
                }

                LinkLabel {
                    id: todoDetailsViewTitleEdit
                    x: breadCrump.model ? ( breadCrump.model.length + 1 ) * 20 : 0
                    label: todoDetailsView.todo ? todoDetailsView.todo.title : ""
                    font.bold: true
                    font.pointSize: fonts.h1
                    width: parent.width - x
                    editable: true

                    onEditFinished: {
                        todoDetailsView.todo.title = todoDetailsViewTitleEdit.label;
                    }

                    Behavior on x { NumberAnimation { duration: 300 } }
                }

                Flow {
                    spacing: 10
                    width: parent.width
                    anchors.horizontalCenter: parent.horizontalCenter
                    move: Transition {
                        NumberAnimation {
                            properties: "x,y"
                            duration: 200
                        }
                    }

                    ProgressIndicator {
                        id: progressIndicator
                        percentage: todoDetailsView.todo ? todoDetailsView.todo.progress : 0

                        onDecreasePressed: todoDetailsView.todo.progress = stepDown()
                        onIncreasePressed: todoDetailsView.todo.progress = stepUp()
                    }
                    PriorityIndicator {
                        id: priorityIndicator
                        width: progressIndicator.width
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

                TodoView {
                    autoSize: true
                    width: parent.width
                    model: todoDetailsView.model
                    allowAddTodos: true
                    parentTodo: todoDetailsView.todo
                    onTodoSelected: todoDetailsView.todo = todo
                }

                Item {
                    width: parent.width
                    height: childrenRect.height

                    BorderImage {
                        width: todoDescriptionEditContains.width
                        height: todoDescriptionEditContains.height
                        source: "../res/description.sci"
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
                            font.pointSize: fonts.p
                            wrapMode: Text.Wrap
                            selectByMouse: true
                            width: parent.width - 40
                            x: 20
                            y: 20

                            onTextChanged: if ( todoDetailsView.todo )
                                               saveDescriptionTimer.delayedSave( todoDetailsView.todo, text );
                        }
                        Timer {
                            id: saveDescriptionTimer
                            interval: 10000

                            property QtObject todo: todo
                            property string text: text

                            function delayedSave( todo, text ) {
                                if ( saveDescriptionTimer.todo ) {
                                    if ( saveDescriptionTimer.todo !== todo ) {
                                        // Other todo --> save immediately
                                        save();
                                    }
                                }
                                saveDescriptionTimer.todo = todo;
                                saveDescriptionTimer.text = text;
                                restart();
                            }

                            function save() {
                                if ( todo ) {
                                    todo.description = text;
                                    todo = null;
                                    text = "";
                                }
                            }

                            onTriggered: save();
                            Component.onDestruction: save();
                        }
                    }
                }
            }
        }
    }
}
