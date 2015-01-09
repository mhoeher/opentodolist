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

import QtQuick 2.4
import QtQuick.Controls 1.2
import "../components" as Components
import "../style" as Style

Components.Page {
    id: aboutPage
    title: qsTr( "About OpenTodoList" )
    ScrollView {
        anchors.fill: parent
        Flickable {
            contentHeight: column.height
            Column {
                id: column
                width: parent.width
                anchors {
                    left: parent.left
                    right: parent.right
                    top: parent.top
                    margins: Style.Measures.midSpace
                }
                spacing: Style.Measures.midSpace

                Label {
                    text: qsTr( "OpenTodoList") + " " + Qt.application.version
                    font.bold: true
                    width: parent.width
                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                }

                Label {
                    text: qsTr( "An Open Source todo and task management application.")
                    width: parent.width
                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                }

                Label {
                    text: qsTr( "Copyright 2013 - 2015, Martin Hoeher" )
                    width: parent.width
                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                }

                Label {
                    text: "<a href='http://www.rpdev.net/home/project/opentodolist'>http://www.rpdev.net/home/project/opentodolist</a>"
                    width: parent.width
                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                    onLinkActivated: Qt.openUrlExternally(link)
                }
            }
        }
    }
}

