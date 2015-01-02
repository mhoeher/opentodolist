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
import net.rpdev.OpenTodoList.DataModel 1.0
import net.rpdev.OpenTodoList.DataBase 1.0
import net.rpdev.OpenTodoList.Models 1.0
import net.rpdev.OpenTodoList.Theme 1.0
import net.rpdev.OpenTodoList.Components 1.0
import net.rpdev.OpenTodoList.Views 1.0

Item {
    id: header

    property TodoModel todos
    property bool trashView: false
    property alias headerText: barLabel.text
    property alias headerFont: barLabel.font

    function add() {
        if ( newTodoName.text !== "" ) {
            d.createTodo( newTodoName.text );
            newTodoName.text = "";
        }
    }

    width: view.width
    height: buttonBar.height + Measures.tinySpace
    clip: true

    Component {
        id: createTodoHelper
        Item {
            property Todo todo : Todo {}
            property DatabaseConnection dbConnection : DatabaseConnection {
                database: application.database
            }
        }
    }

    QtObject {
        id: d

        property bool canCreateTodos: {
            if ( todos.todoList ) {
                return true;
            } else {
                return false;
            }
        }

        function inputToTodoProperties( input ) {
            var properties = {};
            if ( input.length === 0 ) {
                return;
            }
            var dueTodayRegExp = new RegExp( qsTr( "^\\s*(?:due|until)?\\s*today\\s*:\\s*(.+)$" ), "i" );
            var dueTomorrowRegExp = new RegExp( qsTr( "^\\s*(?:due|until)?\\s*tomorrow\\s*:\\s*(.+)$" ), "i" );
            var dueThisWeekRegExp = new RegExp( qsTr( "^\\s*(?:(?:due|until)\\s+)?this\\s+week\\s*:\\s*(.+)$" ), "i" );
            var dueNextWeekRegExp = new RegExp( qsTr( "^\\s*(?:(?:due|until)\\s+)?next\\s+week\\s*:\\s*(.+)$" ), "i" );
            var dueToRegExp = new RegExp( qsTr( "^(?:(?:due|until)\\s+)?([^:]+):\\s*(.+)$" ), "i" );

            var date;
            var off;

            var match = dueTodayRegExp.exec( input );
            if ( match ) {
                properties.dueDate = new Date();
                properties.title = match[1];
                return properties;
            }

            match = dueTomorrowRegExp.exec( input );
            if ( match ) {
                date = new Date();
                date.setDate( date.getDate() + 1 );
                properties.dueDate = date;
                properties.title = match[1];
                return properties;
            }

            match = dueThisWeekRegExp.exec( input );
            if ( match ) {
                var lastDayThisWeek = new Date();
                date = lastDayThisWeek.getDate();
                off = ( 6 - lastDayThisWeek.getDay() + Qt.locale().firstDayOfWeek ) % 7;
                lastDayThisWeek.setDate( date + off );
                properties.dueDate = lastDayThisWeek;
                properties.title = match[1];
                return properties;
            }

            match = dueNextWeekRegExp.exec( input );
            if ( match ) {
                var lastDayNextWeek = new Date();
                date = lastDayNextWeek.getDate();
                off = ( 6 - lastDayNextWeek.getDay() + Qt.locale().firstDayOfWeek ) % 7;
                lastDayNextWeek.setDate( date + off + 7 );
                properties.dueDate = lastDayNextWeek;
                properties.title = match[1];
                return properties;
            }

            match = dueToRegExp.exec( input );
            if ( match ) {
                var dueDate =new Date( Date.parse( match[1] ) );
                if ( !isNaN(dueDate.getFullYear()) &&
                        !isNaN(dueDate.getMonth()) &&
                        !isNaN(dueDate.getDate()) ) {
                    properties.dueDate = dueDate;
                    properties.title = match[2];
                    return properties;
                }
            }

            properties.title = input;
            return properties;
        }

        function createTodo( title ) {
            var properties = inputToTodoProperties( title );
            if ( todos && properties ) {
                var helper = createTodoHelper.createObject( header );
                helper.todo.title = properties.title;
                helper.todo.todoList = todos.todoList.uuid;
                if ( properties.dueDate ) {
                    helper.todo.dueDate = properties.dueDate;
                }
                helper.dbConnection.insertTodo( helper.todo );
                helper.destroy();
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

        Label {
            id: barLabel
            font.bold: true
            text: ""
            x: Measures.tinySpace
            y: Measures.tinySpace
            width: parent.width - 3 * Measures.tinySpace - showSortBarButton.width
            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
            height: buttonFlow.height
            verticalAlignment: Text.AlignVCenter
        }

        Flow {
            id: buttonFlow

            width: Math.max( parent.width - barLabel.contentWidth - 2 * Measures.tinySpace,
                            showSortBarButton.width + 2 * Measures.tinySpace )
            anchors.right: parent.right
            anchors.top: parent.top
            spacing: Measures.tinySpace
            anchors.margins: Measures.tinySpace
            layoutDirection: Qt.RightToLeft

            SymbolLink {
                id: showSortBarButton
                symbol: Symbols.sortByValue
                onClicked: {
                    if ( buttonBar.state === "" ) {
                        buttonBar.state = "hidden";
                        sortBar.state = "showing";
                    }
                }
            }
            SymbolLink {
                symbol: Symbols.search
                onClicked: {
                    if ( buttonBar.state === "" ) {
                        buttonBar.state = "hidden";
                        filterBar.state = "showing";
                    }
                }
            }
            SymbolLink {
                symbol: Symbols.plus
                visible: d.canCreateTodos
                onClicked: {
                    if ( buttonBar.state === "" ) {
                        buttonBar.state = "hidden";
                        addTodoBar.state = "showing";
                    }
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
                right: closeFilterBar.left
                top: parent.top
                margins: Measures.tinySpace
            }
            checked: ViewSettings.showDoneTodos
            visible: !trashView

            onClicked: ViewSettings.showDoneTodos = !ViewSettings.showDoneTodos
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
