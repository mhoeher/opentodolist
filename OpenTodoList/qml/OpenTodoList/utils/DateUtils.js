/*
 *  OpenTodoList - A todo and task manager
 *  Copyright (C) 2014 - 2015 Martin Höher <martin@rpdev.net>
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

.pragma library

/**
  @brief Parses a string and returns properties for a new todo to be created

  This will parse a string as input as title for a new todo. Basically, this will
  return a map which contains the "title" for the new todo. In addition,
  if the input string is matching certain patterns, a "dueDate" will also be
  part of the returned map.
  */
function inputToTodoProperties( input ) {
    var properties = {};
    if ( input.length === 0 ) {
        return;
    }
    var dueTodayRegExp = new RegExp(
                qsTr( "^\\s*(?:due|until)?\\s*today\\s*:\\s*(.+)$" ), "i" );
    var dueTomorrowRegExp = new RegExp(
                qsTr( "^\\s*(?:due|until)?\\s*tomorrow\\s*:\\s*(.+)$" ), "i" );
    var dueThisWeekRegExp = new RegExp(
                qsTr( "^\\s*(?:(?:due|until)\\s+)?this\\s+week\\s*:\\s*(.+)$" ), "i" );
    var dueNextWeekRegExp = new RegExp(
                qsTr( "^\\s*(?:(?:due|until)\\s+)?next\\s+week\\s*:\\s*(.+)$" ), "i" );
    var dueToRegExp = new RegExp(
                qsTr( "^(?:(?:due|until)\\s+)?([^:]+):\\s*(.+)$" ), "i" );

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

function isValidDate(date) {
    return !(isNaN( date.getFullYear() ) ||
             isNaN( date.getMonth() ) ||
             isNaN( date.getDate() ) );
}

function lastDayOfWeekForDate(forThis) {
    var result = new Date( forThis );
    var date = result.getDate();
    var offset = ( 6 - result.getDay() + Qt.locale().firstDayOfWeek ) % 7;
    result.setDate( date + offset );
    return result;
}

function previousDayForDate(forThis) {
    var result = new Date( forThis );
    var date = result.getDate();
    --date;
    result.setDate( date );
    return result;
}

function nextDayForDate(forThis) {
    var result = new Date( forThis );
    var date = result.getDate();
    ++date;
    result.setDate( date );
    return result;
}

var ScheduleGroups = {
    OverdueGroup: "overdue",
    TodayGroup: "today",
    ThisWeekGroup: "week",
    LaterGroup: "later",

    toText: function(group) {
        switch ( group ) {
        case ScheduleGroups.OverdueGroup: return qsTr( "Overdue" );
        case ScheduleGroups.TodayGroup: return qsTr( "Due Today" );
        case ScheduleGroups.ThisWeekGroup: return qsTr( "Due this Week" );
        case ScheduleGroups.LaterGroup: return qsTr( "Scheduled for later" );
        default: return null;
        }
    },

    toColor: function(group,baseColor,altColor) {
        switch ( group ) {
        case ScheduleGroups.OverdueGroup: return baseColor;
        case ScheduleGroups.TodayGroup: return Qt.darker(baseColor,1.1);
        case ScheduleGroups.ThisWeekGroup: return Qt.darker(baseColor,1.2)
        case ScheduleGroups.LaterGroup: return altColor;
        default: return "black";
        }
    }
}

function todoScheduledGroup(todo,forToday) {
    if ( !todo.done && isValidDate(todo.dueDate) ) {
        var today = forToday ? forToday : new Date();
        var yesterday = previousDayForDate(today);
        var endOfWeek = lastDayOfWeekForDate(today);
        if ( todo.dueDate < yesterday ) {
            return ScheduleGroups.OverdueGroup
        }
        if ( todo.dueDate < today ) {
            return ScheduleGroups.TodayGroup;
        }
        if ( todo.dueDate < endOfWeek ) {
            return ScheduleGroups.ThisWeekGroup;
        }
        return ScheduleGroups.LaterGroup;
    }
    return null;
}

function todoScheduledText(todo,forToday,defaultFunction) {
    var result = todoScheduledGroup(todo,forToday);
    if ( !result ) {
        if ( defaultFunction ) {
            return defaultFunction( todo );
        }
    } else {
        result = ScheduleGroups.toText( result );
    }

    return result;
}
