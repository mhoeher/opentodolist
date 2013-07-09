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

var PriorityColors = new Array( 
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
    "#BF0E00" );
PriorityColors[-1] = "#00000000";

function tintPriority( color, priority ) {
    if ( priority < 0 ) {
        return color;
    }
    return Qt.tint( color, "#30" + PriorityColors[priority].substr( 1 ) );
}

var ToolButtonImages = new Array(
    "toolbutton_1.png",
    "toolbutton_2.png",
    "toolbutton_4.png",
    "toolbutton_3.png"
);

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

function getLastDateOfWeek() {
    var current = new Date();
    return new Date(
                current.setDate( current.getDate() - current.getDay() + 6 ) );
}
