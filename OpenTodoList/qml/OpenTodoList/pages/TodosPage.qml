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

import QtQuick 2.4
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import QtQuick.Dialogs 1.2
import "../components" as Components
import "../style" as Style
import "../app" as App
import "../utils/DateUtils.js" as DateUtils
import net.rpdev.OpenTodoList.DataModel 1.0
import net.rpdev.OpenTodoList.DataBase 1.0
import net.rpdev.OpenTodoList.Models 1.0

Components.Page {
    id: todosPage

    property TodoList todoList: null
    property bool searching: false
    property string searchString: ""
    property alias groupingFunction: todoModel.groupingFunction
    property alias showOnlyScheduled: todoModel.showOnlyScheduled
    property alias sortMode: todoModel.sortMode
    property alias showDone: todoModel.showDone

    readonly property var defaultGroupingFunction: function( todo ) {
        if ( todo.done ) {
            return strings.completedTodos
        }
    }

    function refreshGroups() {
        todoModel.rerunGroupingFunction()
    }

    signal todoSelected(Todo todo)

    title: todoList ? todoList.name : qsTr( "Todos" )
    width: 100
    height: 62

    QtObject {
        id: strings
        readonly property string completedTodos: qsTr( "Completed Todos" )
    }

    Timer {
        interval: 1000*60*5 // update "today" every 5 minutes
        running: true
        onTriggered: d.today = new Date()
    }

    Menu {
        id: contextMenu
        title: qsTr( "Todo" )

        MenuItem {
            text: qsTr( "Rename" )
            onTriggered: d.renaming = true
        }
        MenuItem {
            text: qsTr( "Delete" )
            onTriggered: confirmDeleteDialog.open()
        }
    }

    MessageDialog {
        id: confirmDeleteDialog
        title: qsTr( "Delete Todo" )
        text: qsTr( "Deleting the todo will remove " +
                   "it and any contained tasks. Are you " +
                   "sure you want to continue?" )
        standardButtons: StandardButton.Yes | StandardButton.Cancel
        onYes: dbConnection.disposeTodo(d.currentTodo)
    }

    MessageDialog {
        id: confirmPurgeCompletedDialog
        title: qsTr( "Purge Completed Todos")
        text: qsTr( "Do you want to proceed to delete todos that are " +
                   "marked as completed? Note that this cannot be undone." )
        standardButtons: StandardButton.Ok | StandardButton.Cancel
        onAccepted: {
            for ( var i = 0; i < todoModel.objects.length; ++i ) {
                var todo = todoModel.objects[i];
                if ( todo.done ) {
                    dbConnection.disposeTodo(todo);
                }
            }
        }
    }

    DatabaseConnection {
        id: dbConnection
        database: application.database
    }

    QtObject {
        id: d

        property Todo currentTodo: null
        property bool renaming: false
        property date today: new Date()

        function showContextMenu( todo ) {
            currentTodo = todo;
            renaming = false;
            contextMenu.popup();
        }

    }

    TodoModel {
        id: todoModel
        database: (!searching || searchString !== "") ? application.database : null
        showDone: true
        todoList: todosPage.todoList
        filter: searchString
        sortMode: App.GlobalSettings.defaultTodoSortMode
        onDatabaseChanged: {
            if ( searching ) {
                if ( database === null ) {
                    clear();
                }
            }
        }
        groupingFunction: todosPage.defaultGroupingFunction
    }

    Component {
        id: viewDelegate

        DropArea {
            id: dropArea
            width: parent.width
            height: !display.done || App.GlobalSettings.showDoneTodos ? item.height : 0
            keys: ["Todo." + todosPage.groupingFunction(display)]
            z: item.Drag.active ? 3 : 1
            clip: !item.Drag.active

            function dropTodo( todo ) {
                if ( drag.y < height / 2 ) {
                    todoModel.moveTodo( todo, TodoModel.MoveTodoBefore, display );
                } else {
                    todoModel.moveTodo( todo, TodoModel.MoveTodoAfter, display );
                }
            }

            Item {
                id: item
                width: parent.width
                height: Math.max(
                            Math.max( label.height,
                                      edit.height,
                                      checkBox.height ) + Style.Measures.midSpace,
                            Style.Measures.optButtonHeight )
                opacity: display.done ? 0.5 : 1.0
                Drag.keys: ["Todo." + todosPage.groupingFunction(display)]
                Drag.active: dragger.drag.active
                Drag.hotSpot.y: height / 2

                Components.ItemBox {
                    id: background
                    anchors.fill: parent
                }

                MouseArea {
                    id: mouseArea
                    anchors.fill: parent
                    acceptedButtons: Qt.LeftButton | Qt.RightButton
                    onClicked: {
                        switch ( mouse.button ) {
                        case Qt.LeftButton:
                            todosPage.todoSelected(display);
                            break;
                        case Qt.RightButton:
                            d.showContextMenu( display )
                            break;
                        default:
                            break;
                        }
                    }
                    onPressAndHold: {
                        d.showContextMenu( display )
                    }
                }
                Components.Symbol {
                    id:  checkBox
                    anchors {
                        left: parent.left
                        margins: Style.Measures.smallSpace
                        verticalCenter: parent.verticalCenter
                    }
                    font.family: Style.Fonts.symbols.name
                    symbol: display.done ? Style.Symbols.checkedBox : Style.Symbols.uncheckedBox
                    onClicked: display.done = !display.done
                }
                Style.P {
                    id: label
                    visible: !edit.visible
                    text: display.title
                    anchors {
                        left: checkBox.right
                        right: overdueIndicator.left
                        verticalCenter: parent.verticalCenter
                        margins: Style.Measures.smallSpace
                    }
                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                }

                TextField {
                    id: edit
                    text: display.title
                    anchors {
                        left: checkBox.right
                        right: overdueIndicator.left
                        verticalCenter: parent.verticalCenter
                        margins: Style.Measures.smallSpace
                    }
                    visible: d.currentTodo === display && d.renaming
                    validator: RegExpValidator { regExp: /.+/ }

                    onVisibleChanged: {
                        if ( visible ) {
                            text = display.title;
                            focus = true;
                            forceActiveFocus();
                        }
                    }
                    onEditingFinished: {
                        d.renaming = false;
                        focus = false;
                    }
                    onAccepted: {
                        display.title = edit.text;
                    }
                }
                Components.Symbol {
                    id: overdueIndicator

                    property var scheduledGroup: DateUtils.todoScheduledGroup(display,d.today)

                    anchors {
                        right: priorityIndicator.left
                        verticalCenter: parent.verticalCenter
                    }
                    visible: scheduledGroup
                    color: DateUtils.ScheduleGroups.toColor(
                               scheduledGroup,
                               Style.Colors.secondary2,
                               Style.Colors.primary)
                    symbol: Style.Symbols.clock
                }
                Components.OverlayedSymbol {
                    id: priorityIndicator
                    anchors {
                        right: dragger.left
                        verticalCenter: parent.verticalCenter
                    }
                    backSymbol: Style.Symbols.flag
                    frontSymbol: Style.Symbols.flagO
                    backColor: Qt.lighter(
                                   Style.Colors.colorForPriority(display.priority) )
                    visible: display.priority >= 0
                }
                Components.Symbol {
                    id: dragger
                    anchors {
                        right: parent.right
                        verticalCenter: parent.verticalCenter
                        margins: Style.Measures.smallSpace
                    }
                    drag.target: item
                    drag.axis: Drag.YAxis
                    color: Style.Colors.midText
                    symbol: enabled ? Style.Symbols.move : ""
                    enabled: todoModel.sortMode === TodoModel.SortTodoByWeight

                    onReleased: {
                        item.y = 0;
                        if ( item.Drag.target && item.Drag.target !== dropArea ) {
                            item.Drag.target.dropTodo( display )
                        }
                    }
                    onCanceled: item.y = 0;
                }
            }
        }
    }

    Component {
        id: sectionHeaderDelegate
        Item {
            width: parent.width
            height: Math.max( sectionLabel.height,
                             Style.Measures.optButtonHeight,
                             toggleShowDoneButton.height ) +
                    Style.Measures.midSpace
            Style.H5 {
                id: sectionLabel
                z: 2
                text: section
                anchors {
                    left: parent.left
                    right: purgeCompletedTodosButton.left
                    margins: Style.Measures.tinySpace
                    verticalCenter: parent.verticalCenter
                }

                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
            }
            Components.Symbol {
                id: purgeCompletedTodosButton
                enabled: section === strings.completedTodos
                visible: enabled
                font.pointSize: Style.Fonts.h5
                color: Style.Colors.secondary2
                anchors {
                    verticalCenter: parent.verticalCenter
                    right: toggleShowDoneButton.left
                    margins: Style.Measures.tinySpace
                }
                symbol: Style.Symbols.trash
                onClicked: confirmPurgeCompletedDialog.open()
            }
            Components.Symbol {
                id: toggleShowDoneButton
                enabled: section === strings.completedTodos
                visible: enabled
                font.pointSize: Style.Fonts.h5
                anchors {
                    verticalCenter: parent.verticalCenter
                    right: parent.right
                    margins: Style.Measures.tinySpace
                }
                symbol: App.GlobalSettings.showDoneTodos ?
                            Style.Symbols.eye : Style.Symbols.eyeSlash
                onClicked: App.GlobalSettings.showDoneTodos = !App.GlobalSettings.showDoneTodos
            }
        }
    }

    Component {
        id: headerDelegate
        Rectangle {
            id: header
            width: parent.width
            clip: true
            color: Style.Colors.primary
            z: 3
            height: Math.max( newTodoTitle.height,
                             plusButton.height ) +
                    Style.Measures.midSpace

            TextField {
                id: newTodoTitle
                placeholderText: qsTr( "Add Todo" )
                anchors {
                    left: parent.left
                    right: plusButton.left
                    verticalCenter: parent.verticalCenter
                    margins: Style.Measures.tinySpace
                }
                validator: RegExpValidator { regExp: /.+/ }
                onAccepted: plusButton.createTodo()
                onEditingFinished: focus = false
            }

            Components.Symbol {
                id: plusButton
                symbol: Style.Symbols.plus
                color: Style.Colors.lightText
                font.pointSize: Style.Fonts.h4
                anchors {
                    right: parent.right
                    margins: Style.Measures.tinySpace
                    verticalCenter: parent.verticalCenter
                }
                enabled: newTodoTitle.acceptableInput
                onClicked: createTodo()

                function createTodo() {
                    var newTitle = newTodoTitle.text;
                    var newTodoProperties = DateUtils.inputToTodoProperties(newTitle);
                    if ( todosPage.todoList && newTodoProperties.title !== "" ) {
                        var newTodo = newTodoComponent.createObject();
                        newTodo.title = newTodoProperties.title;
                        if ( newTodoProperties.dueDate ) {
                            newTodo.dueDate = newTodoProperties.dueDate;
                        }
                        newTodo.todoList = todosPage.todoList.uuid;
                        dbConnection.insertTodo( newTodo );
                        newTodo.destroy();
                        newTodoTitle.text = "";
                    }
                    newTodoTitle.focus = true;
                    newTodoTitle.forceActiveFocus();
                }
            }

            Component {
                id: newTodoComponent
                Todo {}
            }

            states: State {
                name: "hidden"
                when: todosPage.todoList === null
                PropertyChanges {
                    target: header
                    height: 0
                }
            }
        }
    }

    ScrollView {
        anchors.fill: parent

        ListView {
            id: listView
            model: todoModel
            delegate: viewDelegate
            header: headerDelegate
            headerPositioning: ListView.PullBackHeader
            section {
                property: "group"
                delegate: sectionHeaderDelegate
            }
            displaced: Transition {
                NumberAnimation {
                    properties: "y"
                    duration: 750
                    easing.type: Easing.OutBounce
                }
            }
        }
    }

}

