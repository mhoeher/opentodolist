/*
 *  OpenTodoList - A todo and task manager
 *  Copyright (C) 2014 - 2015  Martin Höher <martin@rpdev.net>
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
import QtQuick.Window 2.2
import QtQuick.Controls 1.3
import QtQuick.Controls.Styles 1.3
import QtQuick.Layouts 1.1

import net.rpdev.OpenTodoList.Core 1.0
import net.rpdev.OpenTodoList.DataModel 1.0
import net.rpdev.OpenTodoList.Models 1.0

import "style" as Style
import "components" as Components
import "pages" as Pages
import "app" as App
import "utils/DateUtils.js" as DateUtils

ApplicationWindow {
    id: root

    width: 800
    height: 600

    Component.onCompleted: {
        application.handler.requestShow.connect( function() { show(); raise(); } );
        application.handler.requestHide.connect( function() { application.hideWindow(); } );
        application.handler.requestToggleWindow.connect( function() {
            if ( visible ) {
                hide();
            } else {
                show();
                raise();
            }
        } );

        width = settings.getValue( "OpenTodoList/Window/width", width );
        height = settings.getValue( "OpenTodoList/Window/height", height );
        x = settings.getValue( "OpenTodoList/Window/x",  x);
        y = settings.getValue( "OpenTodoList/Window/y", y );
        App.GlobalSettings.defaultTodoSortMode = settings.getValue(
                    "OpenTodoList/GlobalSettings/DefaultTodoSortMode",
                    App.GlobalSettings.defaultTodoSortMode );
        App.GlobalSettings.showDoneTodos = settings.getValue(
                    "OpenTodoList/GlobalSettings/ShowDoneTodos", true );
        menusVisible.checked = settings.getValue( "OpenTodoList/Window/MenusVisible",
                                                 menusVisible.checked );

        widthChanged.connect( function() {
            settings.setValue( "OpenTodoList/Window/width", width )
        });
        heightChanged.connect( function() {
            settings.setValue( "OpenTodoList/Window/height", height )
        });
        xChanged.connect( function() {
            settings.setValue( "OpenTodoList/Window/x", x )
        });
        yChanged.connect( function() {
            settings.setValue( "OpenTodoList/Window/y", y )
        });
        App.GlobalSettings.defaultTodoSortModeChanged.connect( function() {
            settings.setValue( "OpenTodoList/GlobalSettings/DefaultTodoSortMode",
                              App.GlobalSettings.defaultTodoSortMode );
        });
        App.GlobalSettings.showDoneTodosChanged.connect( function() {
            settings.setValue( "OpenTodoList/GlobalSettings/ShowDoneTodos",
                              App.GlobalSettings.showDoneTodos );
        });
    }

    onActiveFocusItemChanged: defaultFocusHandler.ensureAppIsHandlingKeys()

    Settings {
        id: settings
    }

    menuBar: MenuBar {
        Menu {
            id: applicationMenu
            title: "&" + Qt.application.name
            visible: menusVisible.checked || Qt.platform.os == "osx"
            Menu {
                title: qsTr( "&Navigate" )
                MenuItem {
                    text: qsTr( "Go &Back" )
                    shortcut: StandardKey.Back
                    onTriggered: stackView.pop()
                }
                MenuSeparator {}
                MenuItem {
                    text: qsTr( "Todo Lists" )
                    shortcut: qsTr( "Ctrl+L" )
                    onTriggered: stackView.showTodoLists()
                }
                MenuItem {
                    text: qsTr( "Search" )
                    shortcut: StandardKey.Find
                    onTriggered: stackView.showSearch()
                }
                MenuItem {
                    text: qsTr( "Scheduled Todos" )
                    shortcut: qsTr( "Ctrl+Alt+S" )
                    onTriggered: stackView.showScheduled()
                }
                MenuItem {
                    text: qsTr( "About" )
                    shortcut: StandardKey.HelpContents
                    onTriggered: stackView.showAbout()
                }
                MenuSeparator {}
                MenuItem {
                    text: qsTr( "Show &Navigation" )
                    onTriggered: navBar.toggle()
                }
            }
            Menu {
                title: qsTr( "&View" )
                MenuItem {
                    id: menusVisible
                    visible: Qt.platform.os !== "osx"
                    text: qsTr( "Show Menu Bar" )
                    checkable: true
                    checked: false
                    shortcut: qsTr( "Ctrl+M" )
                    onCheckedChanged: {
                        settings.setValue( "OpenTodoList/Window/MenusVisible",
                                                        menusVisible.checked )
                        console.debug( "Test!" );
                    }
                }
                MenuItem {
                    text: qsTr( "Show Completed Todos" )
                    shortcut: qsTr( "Ctrl+Alt+D")
                    checkable: true
                    checked: App.GlobalSettings.showDoneTodos
                    onTriggered: App.GlobalSettings.showDoneTodos = !App.GlobalSettings.showDoneTodos
                }
                Menu {
                    title: qsTr( "Sort Todos" )
                    MenuItem {
                        text: qsTr( "By Name" )
                        checkable: true
                        checked: App.GlobalSettings.defaultTodoSortMode === TodoModel.SortTodoByName
                        onTriggered: App.GlobalSettings.defaultTodoSortMode = TodoModel.SortTodoByName
                    }
                    MenuItem {
                        text: qsTr( "By Due Date" )
                        checkable: true
                        checked: App.GlobalSettings.defaultTodoSortMode === TodoModel.SortTodoByDueDate
                        onTriggered: App.GlobalSettings.defaultTodoSortMode = TodoModel.SortTodoByDueDate
                    }
                    MenuItem {
                        text: qsTr( "By Priority" )
                        checkable: true
                        checked: App.GlobalSettings.defaultTodoSortMode === TodoModel.SortTodoByPriority
                        onTriggered: App.GlobalSettings.defaultTodoSortMode = TodoModel.SortTodoByPriority
                    }
                    MenuItem {
                        text: qsTr( "Manually" )
                        checkable: true
                        checked: App.GlobalSettings.defaultTodoSortMode === TodoModel.SortTodoByWeight
                        onTriggered: App.GlobalSettings.defaultTodoSortMode = TodoModel.SortTodoByWeight
                    }
                }
            }
            Menu {
                title: qsTr( "&Development Tools" )
                visible: application.isDebugBuild
                MenuItem {
                    text: qsTr( "Print Current Focus Item" )
                    shortcut: "Ctrl+Shift+P"
                    onTriggered: {
                        var cFI = activeFocusItem;
                        console.debug( "Current Focus item is " + cFI +
                                      "[" + cFI.objectName + "]" )
                        while ( cFI.parent !== null ) {
                            cFI = cFI.parent;
                            console.debug( "  --> " + cFI + "[" + cFI.objectName + "]" )
                        }
                    }
                }
                MenuItem {
                    text: qsTr( "Unset Current Focus Item" )
                    onTriggered: {
                        defaultFocusHandler.focus = true;
                        defaultFocusHandler.forceActiveFocus()
                        console.debug( "Current Focus Item: " +
                                      root.activeFocusItem );
                    }
                }
            }
            MenuItem {
                shortcut: StandardKey.Close
                text: qsTr( "&Close Window" )
                onTriggered: application.handler.hideWindow()
            }
            MenuItem {
                shortcut: StandardKey.Quit
                text: qsTr( "&Quit" )
                onTriggered: application.handler.terminateApplication()
            }
        }
    }

    toolBar: ToolBar {
        style: ToolBarStyle {
            background: Rectangle {
                color: Style.Colors.primary
            }
        }

        RowLayout {
            anchors.fill: parent

            Components.Symbol {
                id: toggleNavButton
                symbol: stackView.depth === 1 ? Style.Symbols.bars : Style.Symbols.singleLeft
                font.pointSize: Style.Fonts.h1
                color: Style.Colors.lightText
                onClicked: {
                    if ( stackView.depth === 1 ) {
                        navBar.toggle()
                    } else {
                        stackView.pop()
                    }
                }
            }
            Style.H1 {
                text: stackView.currentItem ? stackView.currentItem.title : ""
                Layout.fillWidth: true
                elide: Text.ElideRight
                color: Style.Colors.lightText
            }
            Components.Symbol {
                symbol: Style.Symbols.textEditCopy
                font.pointSize: Style.Fonts.h1
                color: Style.Colors.lightText
                visible: !!activeFocusItem && !!activeFocusItem["copy"]
                onClicked: activeFocusItem.copy()
            }
            Components.Symbol {
                symbol: Style.Symbols.textEditCut
                font.pointSize: Style.Fonts.h1
                color: Style.Colors.lightText
                visible: !!activeFocusItem && !!activeFocusItem["cut"]
                onClicked: activeFocusItem.cut()
            }
            Components.Symbol {
                symbol: Style.Symbols.textEditPaste
                font.pointSize: Style.Fonts.h1
                color: Style.Colors.lightText
                visible: !!activeFocusItem && !!activeFocusItem["paste"]
                onClicked: activeFocusItem.paste()
            }
            TextField {
                id: searchEdit
                visible: activeFocus
                placeholderText: qsTr( "Search..." )
                onTextChanged: {
                    if ( stackView.currentItem &&
                         typeof(stackView.currentItem["searchString"]) === "string" ) {
                        stackView.currentItem.searchString = text ? text : "";
                    }
                }
            }
            Components.Symbol {
                symbol: Style.Symbols.search
                font.pointSize: Style.Fonts.h1
                color: Style.Colors.lightText
                visible: stackView.currentItem &&
                         typeof(stackView.currentItem["searchString"]) === "string"
                onClicked: {
                    if ( searchEdit.activeFocus ) {
                        defaultFocusHandler.focus = true;
                        defaultFocusHandler.forceActiveFocus();
                    } else {
                        searchEdit.focus = true;
                        searchEdit.forceActiveFocus();
                    }
                }
            }
            Components.Symbol {
                symbol: Style.Symbols.verticalEllipsis
                font.pointSize: Style.Fonts.h1
                color: Style.Colors.lightText
                onClicked: applicationMenu.popup()
            }
        }
    }

    onClosing: {
        if ( Qt.platform.os === "android" ) {
            if ( stackView.depth > 1 ) {
                defaultFocusHandler.handleBack();
                close.accepted = false;
            } else {
                application.handler.terminateApplication();
            }
        }
    }

    StackView {
        id: stackView
        anchors.fill: parent
        initialItem: todoLists

        function showTodoLists() {
            clear();
            push( todoLists );
            navBar.state = "";
        }

        function showSearch() {
            clear();
            push( searchPage );
            navBar.state = "";
            searchEdit.focus = true;
            searchEdit.forceActiveFocus();
        }

        function showScheduled() {
            clear();
            push( schedulePage );
            navBar.state = "";
        }

        function showAbout() {
            clear();
            push( aboutPage );
            navBar.state = "";
        }

        function showTodo( todo ) {
            var newTodoPage = todoPage.createObject();
            newTodoPage.newTodo.fromVariant(todo.toVariant());
            newTodoPage.todo = newTodoPage.newTodo
            stackView.push( { item: newTodoPage, destroyOnPop: true } );
        }

        onCurrentItemChanged: {
            if ( currentItem && typeof(currentItem["searchString" ]) === "string" ) {
                searchEdit.text = currentItem.searchString;
            } else {
                searchEdit.text = "";
                searchEdit.focus = false;
            }
        }

        Component {
            id: todoLists
            Pages.TodoListsPage {
                onTodoListSelected: {
                    var newTodoListPage = todos.createObject();
                    newTodoListPage.todoList = todoList;
                    stackView.push( newTodoListPage );
                }
            }
        }

        Component {
            id: todos
            Pages.TodosPage {
                onTodoSelected: stackView.showTodo( todo )
            }
        }

        Component {
            id: todoPage
            Pages.TodoPage {
                property Todo newTodo: Todo {
                }
            }
        }

        Component {
            id: searchPage
            Pages.TodosPage {
                title: qsTr( "Search Todos" )
                searching: true
                onTodoSelected: stackView.showTodo( todo )
            }
        }

        Component {
            id: schedulePage
            Pages.TodosPage {
                title: qsTr( "Scheduled Todos" )
                showOnlyScheduled: true
                sortMode: TodoModel.SortTodoByDueDate
                showDone: false
                groupingFunction: function(todo) {
                    if ( todo.done ) {
                        return defaultGroupingFunction(todo);
                    }
                    var today = new Date();
                    var yesterday = DateUtils.previousDayForDate(today);
                    var endOfWeek = DateUtils.lastDayOfWeekForDate(today);
                    if ( todo.dueDate < yesterday ) {
                        return qsTr( "Overdue" )
                    }
                    if ( todo.dueDate < today ) {
                        return qsTr( "Due Today" )
                    }
                    if ( todo.dueDate < endOfWeek ) {
                        return qsTr( "Due this Week" )
                    }
                    return qsTr( "Scheduled for later" )
                }
                onTodoSelected: stackView.showTodo( todo )

                Timer {
                    interval: 1000*60*5 // update every 5 minutes
                    running: true
                    onTriggered: { parent.refreshGroups(); console.debug( "Refreshed" ) }
                }
            }
        }

        Component {
            id: aboutPage
            Pages.AboutPage {}
        }
    }

    Components.NavigationBar {
        id: navBar
        anchors {
            top: parent.top
            bottom: parent.bottom
        }
        width: Math.min( Style.Measures.mWidth * 30, parent.width )
        x: -width
        items: navBarEntries
        itemIdRole: "name"
        itemTitleRole: "title"
        itemGroupRole: "group"

        function toggle() {
            navBar.state = ( navBar.state === "shown" ) ?
                        "" : "shown";
        }

        states: State {
            name: "shown"
            PropertyChanges {
                target: navBar
                x: 0
                focus: true
            }
        }

        Keys.onEscapePressed: state = ""
        Keys.onBackPressed: state = ""

        onItemSelected: {
            switch ( itemId ) {
            case "todoLists":
                stackView.showTodoLists();
                break;
            case "search":
                stackView.showSearch();
                break;
            case "scheduled":
                stackView.showScheduled();
                break;
            case "about":
                stackView.showAbout();
                break;
            case "quit":
                application.handler.terminateApplication();
                break;
            }
        }

        Behavior on x { SmoothedAnimation { duration: 300 } }
    }

    ListModel {
        id: navBarEntries
        ListElement {
            name: "todoLists"
            title: qsTr( "Todo Lists" )
            group: qsTr( "Todos" )
        }
        ListElement {
            name: "search"
            title: qsTr( "Search" )
            group: qsTr( "Todos" )
        }
        ListElement {
            name: "scheduled"
            title: qsTr( "Scheduled Todos" )
            group: qsTr( "Todos" )
        }
        ListElement {
            name: "about"
            title: qsTr( "About" )
            group: qsTr( "Application" )
        }
        ListElement {
            name: "quit"
            title: qsTr( "Quit" )
            group: qsTr( "Application" )
        }
    }

    Item {
        id: defaultFocusHandler
        objectName: "defaultFocusHandler"
        focus: true

        Keys.onBackPressed: handleBack()
        Keys.onEscapePressed: handleBack()

        /**
          Helper function: Checks if the current focus is inside the root window and
          if not ensures that the focus is returned to the window. This is required
          to handle Back and Escape properly.
          */
        function ensureAppIsHandlingKeys() {
            var contentItem = root.contentItem;
            var toolBarContentItem = root.toolBar.contentItem;
            var currentFocusItem = root.activeFocusItem;
            var isHandling = false;
            while ( currentFocusItem !== null ) {
                if ( currentFocusItem === contentItem ||
                     currentFocusItem === toolBarContentItem ) {
                    isHandling = true;
                    break;
                }
                currentFocusItem = currentFocusItem.parent;
            }
            if ( !isHandling ) {
                defaultFocusHandler.focus = true;
                defaultFocusHandler.forceActiveFocus();
            }
        }

        function handleBack() {
            if ( stackView.depth > 1 ) {
                stackView.pop();
            } else {
                switch ( Qt.platform.os ) {
                case "android":
                    application.handler.hideWindow();
                    break;
                default:
                    break;
                }
            }
        }
    }

    BackendModel {
        database: application.database
    }
}

/*Window {
    id: root

    width: 800
    height: 600
    color: Colors.window

    function saveViewSettings() {
        console.debug( "SaveViewSettings" );
        settings.setValue( "OpenTodoList/ViewSettings/todoSortMode",
ViewSettings.todoSortMode );
        settings.setValue( "OpenTodoList/ViewSettings/showDoneTodos",
ViewSettings.showDoneTodos );
    }


    onActiveFocusItemChanged: {
        if ( activeFocusItem === null ) {
            pageStack.focus = true;
        }
    }

    Component.onCompleted: {
        application.handler.requestShow.connect( function() { show(); raise(); } );
        application.handler.requestHide.connect( function() { hide(); } );
        application.handler.requestToggleWindow.connect( function() {
            if ( visible ) {
                hide();
            } else {
                show();
                raise();
            }
        } );

        width = settings.getValue( "OpenTodoList/Window/width", width );
        height = settings.getValue( "OpenTodoList/Window/height", height );
        ViewSettings.todoSortMode = settings.getValue(
"OpenTodoList/ViewSettings/todoSortMode", ViewSettings.todoSortMode );
        ViewSettings.showDoneTodos = settings.getValue(
"OpenTodoList/ViewSettings/showDoneTodos", ViewSettings.showDoneTodos ) === "true";

        ViewSettings.onSettingsChanged.connect( saveViewSettings );
    }

    onWidthChanged: settings.setValue( "OpenTodoList/Window/width", width )
    onHeightChanged: settings.setValue( "OpenTodoList/Window/height", height )

    Settings {
        id: settings
    }

    PageStack {
        id: pageStack

        anchors.fill: parent
        focus: true

        onLastPageClosing: {
            if ( Qt.platform.os === "android" ) {
                // TODO: Stop main activity here
                application.handler.hideWindow()
            }
        }

        TodoListsPage {
            onTodoSelected: {
                var component = Qt.createComponent( "TodoPage.qml" );
                if ( component.status === Component.Ready ) {
                    var page = component.createObject( pageStack );
                    page.todo.fromVariant( todo.toVariant() );
                } else {
                    console.error( component.errorString() );
                }
            }
        }
    }

    Shortcut {
        keySequence: fromStandardKey( StandardKey.Quit )
        onTriggered: {
            application.handler.terminateApplication()
        }
    }

    Rectangle {
        anchors.fill: parent
        color: Style.Style.colors.
    }
}*/

