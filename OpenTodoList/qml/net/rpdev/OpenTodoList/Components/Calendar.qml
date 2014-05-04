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
import QtQuick.Layouts 1.1
import net.rpdev.OpenTodoList.Theme 1.0

Rectangle {
    id: calendar

    property date focusDate: new Date()
    property var highlightDates: []
    property var locale: Qt.locale()

    signal dateClicked( date selectedDate )

    function isHighlighted( date ) {
        var thisDate = Qt.formatDate( date );
        for ( var i = 0; i < highlightDates.length; ++i ) {
            var highlightDate = Qt.formatDate( highlightDates[i] );
            if ( highlightDate === thisDate ) {
                return true;
            }
        }
        return false;
    }

    function isFocusDate( date ) {
        return Qt.formatDate( date ) === Qt.formatDate( focusDate );
    }

    function isCurrentDate( date ) {
        return Qt.formatDate( date ) === Qt.formatDate( new Date() );
    }

    function isWeekDay( date ) {
        var day = date.getDay();
        for ( var i = 0; i < locale.weekDays.length; ++i ) {
            if ( locale.weekDays[ i ] === day ) {
                return true;
            }
        }
        return false;
    }

    QtObject {
        id: d
        property date currentDate: focusDate

        readonly property int daysInMonth: ( new Date(
                                                currentDate.getFullYear(),
                                                currentDate.getMonth() + 1,
                                                0 ) ).getDate()
        readonly property int firstDayInMonth: ( new Date(
                                                    currentDate.getFullYear(),
                                                    currentDate.getMonth(),
                                                    1 ) ).getDay() - locale.firstDayOfWeek
        readonly property int weeksInMonth: Math.ceil( ( daysInMonth + firstDayInMonth ) / 7 )

    }

    width: 800
    height: 600
    gradient: Gradient {
        GradientStop {
            position: 0.0
            color: Colors.button
        }
        GradientStop {
            position: 1.0
            color: Qt.darker( Colors.button, 1.2 )
        }
    }
    border {
        color: Colors.border
        width: Measures.smallBorderWidth
    }

    MouseArea {
        anchors.fill: parent
    }

    Item {
        id: toolBar
        anchors {
            left: parent.left
            right: parent.right
            top: parent.top
            margins: Measures.tinySpace
        }
        height: childrenRect.height

        RowLayout {
            width: parent.width
            height: childrenRect.height

            SymbolButton {
                symbol: Symbols.doubleLeft
                onClicked: d.currentDate = new Date( d.currentDate.getFullYear() - 1, d.currentDate.getMonth(), 1 );
            }
            SymbolButton {
                symbol: Symbols.singleLeft
                onClicked: d.currentDate = new Date( d.currentDate.getFullYear(), d.currentDate.getMonth() - 1, 1 );
            }
            Label {
                text: Qt.formatDate( d.currentDate, "MMMM yyyy" )
                horizontalAlignment: Qt.AlignCenter
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignCenter
            }
            SymbolButton {
                symbol: Symbols.singleRight
                onClicked: d.currentDate = new Date( d.currentDate.getFullYear(), d.currentDate.getMonth() + 1, 1 );
            }
            SymbolButton {
                symbol: Symbols.doubleRight
                onClicked: d.currentDate = new Date( d.currentDate.getFullYear() + 1, d.currentDate.getMonth(), 1 );
            }
        }
    }

    Item {
        id: dayHeader

        anchors {
            left: parent.left
            right: parent.right
            top: toolBar.bottom
            margins: Measures.tinySpace
        }
        height: childrenRect.height

        Flow {
            anchors {
                left: parent.left
                right: parent.right
            }

            Repeater {
                model: 7

                delegate: Item {
                    width: Math.floor( dayHeader.width / 7 )
                    height: childrenRect.height

                    Label {
                        text: locale.dayName( ( index + locale.firstDayOfWeek ) % 7, Locale.ShortFormat )
                        anchors {
                            horizontalCenter: parent.horizontalCenter
                        }
                    }
                }
            }
        }
    }

    Item {
        id: monthView

        anchors {
            left: parent.left
            right: parent.right
            top: dayHeader.bottom
            bottom: parent.bottom
            margins: Measures.tinySpace
        }

        Flow {
            id: flow

            anchors.fill: parent

            Repeater {
                model: d.daysInMonth + d.firstDayInMonth

                delegate: Item {
                    id: delegate

                    property bool inMonth: index - d.firstDayInMonth >= 0
                    property date date: inMonth ? new Date( d.currentDate.getFullYear(),
                                                           d.currentDate.getMonth(),
                                                           index - d.firstDayInMonth + 1 ) : new Date()

                    width: Math.floor( flow.width / 7 )
                    height: Math.floor( flow.height / d.weeksInMonth )

                    Item {
                        anchors.fill: parent
                        visible: delegate.inMonth

                        Rectangle {
                            id: dateBackground
                            anchors {
                                fill: parent
                                margins: Math.min( parent.width, parent.height ) < Measures.mHeight * 2 ?
                                             0 : Measures.tinySpace
                            }
                            color: {
                                if ( isHighlighted( delegate.date ) ) {
                                    return Colors.secondary2;
                                }
                                if ( isCurrentDate( delegate.date ) ) {
                                    return Colors.secondary1;
                                }
                                if ( !isWeekDay( delegate.date ) ) {
                                    return Colors.primary;
                                }
                                return Colors.button;
                            }
                            gradient: Gradient {
                                GradientStop {
                                    position: 0.0
                                    color: dateBackground.color
                                }
                                GradientStop {
                                    position: 1.0
                                    color: Qt.darker( dateBackground.color, 1.1 )
                                }
                            }
                            border {
                                color: Colors.border
                                width: Measures.smallBorderWidth
                            }
                        }

                        Label {
                            anchors {
                                centerIn: parent
                            }

                            text: delegate.date.getDate()
                            color: Qt.colorEqual( dateBackground.color, Colors.button ) ?
                                       Colors.text : Colors.midText
                            font.pixelSize: Math.min( dateBackground.width, dateBackground.height ) / 2
                        }

                        MouseArea {
                            anchors.fill: parent
                            onClicked: calendar.dateClicked( delegate.date )
                        }
                    }
                }
            }
        }
    }
}
