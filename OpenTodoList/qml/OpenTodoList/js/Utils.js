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

var PriorityColors = [
    "#00E508",
    "#24E100",
    "#4FDD00",
    "#78D900",
    "#A0D500",
    "#C6D200",
    "#CEB000",
    "#CA8500",
    "#C65C00",
    "#C23400",
    "#BF0E00" ];
PriorityColors[-1] = "#00000000";

function tintPriority( color, priority ) {
    if ( priority < 0 ) {
        return color;
    }
    return Qt.tint( color, "#30" + PriorityColors[priority].substr( 1 ) );
}

function tint( base, overlay, alpha ) {
    return Qt.tint( base, Qt.rgba( overlay.r, overlay.g, overlay.b, alpha ) )
}

var ToolButtonImages = [
    "toolbutton_1.png",
    "toolbutton_2.png",
    "toolbutton_4.png",
    "toolbutton_3.png"
];

function getToolButtonImage( index ) {
    return ToolButtonImages[ index % ToolButtonImages.length ];
}

function initViewList() {
    return [];
}

function showView( list, view ) {
    if ( !list ) {
        list = [];
    }
    for ( var i = 0; i < list.length; ++i ) {
        list[i].active = false;
    }
    list.push( view );
    view.active = true;
    return list;
}

function hideView( list, view ) {
    view.active = false;
    list.pop();
    if ( list.length > 0 ) {
        list[list.length-1].active = true;
    }
    return list;
}

var DayNamesShort = [ "So", "Mo", "Tu", "We", "Th", "Fr", "Sa" ];
var MonthNames = [ "January", "February", "March",
                   "April", "May", "June", "July",
                  "August", "September", "October",
                  "November", "December" ];

function getLastDateOfWeek() {
    var current = new Date();
    return new Date(
                current.setDate( current.getDate() - current.getDay() + 6 ) );
}

function getFirstDay( date ) {
    return new Date( date.getFullYear(), date.getMonth(), 1 ).getDay();
}

function getDaysInMonth( date ) {
    return new Date( date.getFullYear(), date.getMonth() + 1, 0 ).getDate();
}

function getNullDate() {
    return new Date( NaN, NaN, NaN );
}

function isValidDate( date ) {
    return !( isNaN( date.getFullYear() ) ||
             isNaN( date.getMonth() ) ||
             isNaN( date.getDate() ) );
}

function areDatesEqual( date1, date2 ) {
    return isValidDate( date1 ) &&
           isValidDate( date2 ) &&
           date1.getFullYear() === date2.getFullYear() &&
             date1.getMonth() === date2.getMonth() &&
             date1.getDate() === date2.getDate();
}

function formatDate( date, defaultLabel ) {
    return isValidDate( date ) ?
                date.getDate() + " " +
            MonthNames[ date.getMonth() ] + " " +
            date.getFullYear() :
                defaultLabel;
}

/*
  Applies a given function on each todo in the given model. This is useful
  as due to changing a todo, the ordering on the list of todos might change.
  This is taken into account by this function.
  */
function applyToTodos( model, func ) {
    var todos = [];
    for ( var i = 0; i < model.itemCount(); ++i ) {
        todos.push( model.getItem( i ).object );
    }
    for ( var i = 0; i < todos.length; ++i ) {
        func( todos[ i ] );
    }
}

function recursiveSetTodoProgress( todo, progress ) {
    if ( todo.hasSubTodos() ) {
        applyToTodos( todo.subTodos, function( todo ) {
            recursiveSetTodoProgress( todo, progress );
        } );
    } else {
        todo.progress = progress;
    }
}
