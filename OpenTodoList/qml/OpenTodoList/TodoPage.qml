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
import net.rpdev.OpenTodoList.Core 1.0
import net.rpdev.OpenTodoList.Theme 1.0
import net.rpdev.OpenTodoList.Components 1.0
import net.rpdev.OpenTodoList.Views 1.0

Page {
    id: page

    property Todo todo

    name: todo && todo.title !== "" ? todo.title : qsTr( "Untitled Todo" )

    TabView {
        anchors.fill: parent
        showActionButton: true
        actionButtonTitle: qsTr( "Todo Lists" )

        onActionButtonClicked: page.close()

        Tab {
            name: page.name

            Flickable {
                anchors.fill: parent
                anchors.margins: Measures.smallSpace
                contentWidth: width
                contentHeight: grid.height

                GridLayout {
                    id: grid
                    columns: 2
                    anchors {
                        left: parent.left
                        right: parent.right
                        top: parent.top
                    }

                    SymbolLabel {
                        id: titleLabel
                        symbol: page.todo && page.todo.isDone ? Symbols.checkedBox : Symbols.uncheckedBox
                        text: page.todo ? page.todo.title : ""
                        placeholder: qsTr( "Untitled Todo" )
                        useSymbolButton: true
                        Layout.columnSpan: 2
                        Layout.fillWidth: true
                        Layout.preferredHeight: height

                        onSymbolClicked: if ( page.todo ) page.todo.toggle()
                        onClicked: {
                            var dialog = editor.createObject( this );
                            dialog.show();
                        }

                        property Component editor: Component {
                            LineEditDialog {
                                inputLabel: qsTr( "Rename Todo:" )
                                text: todo.title
                                onTextAvailable: if ( page.todo ) page.todo.title = text
                            }
                        }
                    }

                    Label {
                        text: qsTr( "Progress:" )
                        font.bold: true
                    }
                    Label {
                        text: page.todo ? page.todo.progress + "%" : ""
                        Layout.fillWidth: true
                    }

                    Label {
                        text: qsTr( "Priority:" )
                        font.bold: true
                    }
                    Label {
                        text: page.todo ?
                                  ( page.todo.priority >= 0 ?
                                              page.todo.priority :
                                              qsTr( "None specified" ) ) :
                                  ""
                        Layout.fillWidth: true
                    }

                    Label {
                        text: qsTr( "Due to: " )
                        font.bold: true
                    }
                    Label {
                        text: page.todo ? Qt.formatDateTime( page.todo.dueDate ) : ""
                        Layout.fillWidth: true
                    }

                    Label {
                        text: page.todo ? page.todo.description : ""
                        Layout.columnSpan: 2
                        textFormat: Text.RichText
                    }
                }
            }
        }
    }
}
