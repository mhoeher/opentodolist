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
import net.rpdev.OpenTodoList.Core 1.0
import net.rpdev.OpenTodoList.Theme 1.0
import net.rpdev.OpenTodoList.Components 1.0
import net.rpdev.OpenTodoList.Views 1.0

Item {
    id: header

    property TodoModel todos
    property bool trashView: false

    function add() {
        if ( newTodoName.text !== "" ) {
            d.createTodo( newTodoName.text );
            newTodoName.text = "";
        }
    }

    width: view.width
    height: buttonBar.height + Measures.tinySpace
    clip: true

    QtObject {
        id: d

        property bool canCreateTodos: {
            if ( todos ) {
                switch ( todos.queryType ) {
                case TodoModel.QueryTopLevelTodosInTodoList:
                    return todos.todoList && todos.todoList.canCreateTodos;
                case TodoModel.QuerySubTodosOfTodo:
                    return todos.parentTodo && todos.parentTodo.canCreateTodos;
                default: return false;
                }
            }
            return false;
        }

        function createTodo( title ) {
            if ( todos ) {
                switch ( todos.queryType ) {
                case TodoModel.QueryTopLevelTodosInTodoList:
                    todos.todoList.addTodo( title );
                    break;
                case TodoModel.QuerySubTodosOfTodo:
                    todos.parentTodo.addTodo( title );
                    break;
                default:
                    break;
                }
            }
        }
    }

    Item {
        id: buttonBar

        width: parent.width
        height: Math.max(
                    childrenRect.height,
                    addTodoBar.height,
                    filterBar.height,
                    sortBar.height) + Measures.tinySpace * 2

        Flow {
            width: parent.width
            anchors.right: parent.right
            anchors.top: parent.top
            spacing: Measures.tinySpace
            anchors.margins: Measures.tinySpace
            layoutDirection: Qt.RightToLeft

            SymbolLink {
                symbol: Symbols.sortByValue
                onClicked: {
                    buttonBar.state = "hidden";
                    sortBar.state = "showing";
                }
            }
            SymbolLink {
                symbol: Symbols.search
                onClicked: {
                    buttonBar.state = "hidden";
                    filterBar.state = "showing";
                }
            }
            SymbolLink {
                symbol: Symbols.plus
                visible: d.canCreateTodos
                onClicked: {
                    buttonBar.state = "hidden";
                    addTodoBar.state = "showing";
                }
            }
        }

        states: State {
            name: "hidden"
            PropertyChanges {
                target: buttonBar
                opacity: 0.0
            }
        }

        transitions: [
            Transition {
                from: ""
                to: "hidden"
                reversible: true
                PropertyAnimation {
                    properties: "opacity"
                }
            }
        ]
    }

    Item {
        id: addTodoBar

        width: parent.width
        height: childrenRect.height
        opacity: 0
        visible: opacity > 0
        y: -height

        SingleLineTextInput {
            id: newTodoName

            anchors {
                left: parent.left
                top: parent.top
                right: addTodoButton.left
                margins: Measures.tinySpace
            }
            placeholder: qsTr( "Add new todo..." )

            onAccept: add()
        }

        SymbolButton {
            id: addTodoButton

            symbol: Symbols.plus
            anchors {
                right: closeAddTodoBar.left
                top: parent.top
                margins: Measures.tinySpace
            }

            onClicked: add()
        }

        SymbolLink {
            id: closeAddTodoBar

            symbol: Symbols.close
            anchors {
                right: parent.right
                top: parent.top
                margins: Measures.tinySpace
            }

            onClicked: {
                addTodoBar.state = "";
                buttonBar.state = "";
            }
        }

        states: State {
            name: "showing"
            PropertyChanges {
                target: addTodoBar
                opacity: 1.0
                y: 0
            }
        }

        transitions: [
            Transition {
                from: ""
                to: "showing"
                reversible: true
                PropertyAnimation {
                    properties: "opacity,y"
                }
            }
        ]
    }

    Item {
        id: filterBar

        width: parent.width
        height: childrenRect.height
        opacity: 0.0
        visible: opacity > 0
        y: -height

        SingleLineTextInput {
            id: filterTodosEdit

            placeholder: qsTr( "Filter todos..." )
            anchors {
                left: parent.left
                top: parent.top
                right: showDoneTodos.left
                margins: Measures.tinySpace
            }

            onTextChanged: todos.filter = text
        }

        SymbolButton {
            id: showDoneTodos

            symbol: Symbols.checkedBox
            anchors {
                right: showDeletedTodos.left
                top: parent.top
                margins: Measures.tinySpace
            }
            checked: ViewSettings.showDoneTodos
            visible: !trashView

            onClicked: ViewSettings.showDoneTodos = !ViewSettings.showDoneTodos
        }

        SymbolButton {
            id: showDeletedTodos

            symbol: Symbols.trash
            anchors {
                right: closeFilterBar.left
                top: parent.top
                margins: Measures.tinySpace
            }
            checked: ViewSettings.showDeletedTodos
            visible: !trashView

            onClicked: ViewSettings.showDeletedTodos = !ViewSettings.showDeletedTodos
        }

        SymbolLink {
            id: closeFilterBar

            symbol: Symbols.close
            anchors {
                right: parent.right
                top: parent.top
                margins: Measures.tinySpace
            }

            onClicked: {
                filterBar.state = "";
                buttonBar.state = "";
            }
        }

        states: State {
            name: "showing"
            PropertyChanges {
                target: filterBar
                opacity: 1.0
                y: 0
            }
        }

        transitions: [
            Transition {
                from: ""
                to: "showing"
                reversible: true
                PropertyAnimation {
                    properties: "opacity,y"
                }
            }
        ]
    }

    Item {
        id: sortBar

        width: parent.width
        height: childrenRect.height
        opacity: 0.0
        visible: opacity > 0
        y: -height

        Flow {
            width: parent.width - 2 * Measures.tinySpace
            x: Measures.tinySpace
            y: Measures.tinySpace
            layoutDirection: Qt.RightToLeft
            spacing: Measures.tinySpace

            SymbolLink {
                id: closeSortBarButton

                symbol: Symbols.close

                onClicked: {
                    sortBar.state = "";
                    buttonBar.state = "";
                }
            }

            SymbolButton {
                checked: ViewSettings.todoSortMode === Todo.SortTodoByPercentageDone
                onClicked: ViewSettings.todoSortMode = Todo.SortTodoByPercentageDone
                symbol: Symbols.tasks
            }
            SymbolButton {
                checked: ViewSettings.todoSortMode === Todo.SortTodoByDueDate
                symbol: Symbols.calendarFull
                onClicked: ViewSettings.todoSortMode = Todo.SortTodoByDueDate
            }
            SymbolButton {
                checked: ViewSettings.todoSortMode === Todo.SortTodoByPriority
                symbol: Symbols.exclamationMark
                onClicked: ViewSettings.todoSortMode = Todo.SortTodoByPriority
            }
            SymbolButton {
                checked: ViewSettings.todoSortMode === Todo.SortTodoByName
                symbol: Symbols.sortByName
                onClicked: ViewSettings.todoSortMode = Todo.SortTodoByName
            }
        }

        states: State {
            name: "showing"
            PropertyChanges {
                target: sortBar
                y: 0
                opacity: 1.0
            }
        }

        transitions: [
            Transition {
                from: ""
                to: "showing"
                reversible: true
                PropertyAnimation {
                    properties: "opacity,y"
                }
            }
        ]
    }


}
