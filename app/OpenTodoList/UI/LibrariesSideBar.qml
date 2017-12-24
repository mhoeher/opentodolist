import QtQuick 2.5
import QtQuick.Controls 2.2

import OpenTodoList 1.0
import OpenTodoList.UI 1.0

Item {
    id: sidebar

    property Library currentLibrary: App.libraries[0]
    property string currentTag: ""
    property bool helpVisible: false
    property bool compact: false

    signal libraryClicked(Library library)
    signal newLibrary()
    signal aboutPageRequested()
    signal close()

    Rectangle {
        anchors.fill: parent
        color: Colors.dark
    }

    ScrollView {
        id: scrollView
        anchors.fill: parent

        Connections {
            target: currentLibrary
            onLibraryDeleted: currentLibrary = null
        }

        Component {
            id: libraryItemDelegate

            Column {
                width: parent.width

                ButtonContainer {
                    id: buttonContainer
                    property bool isActive: (currentLibrary === App.libraries[index]) &&
                                            (currentTag === "")
                    mainItem: itemText
                    width: parent.width
                    cursorShape: Qt.PointingHandCursor

                    onClicked: {
                        var library = App.libraries[index];
                        if (currentLibrary !== library) {
                            sidebar.currentLibrary = library;
                        }
                        sidebar.currentTag = "";
                        libraryClicked(App.libraries[index]);
                        sidebar.close();
                    }

                    Rectangle {
                        anchors.fill: parent
                        color: Colors.highlight
                        visible: buttonContainer.isActive
                        opacity: buttonContainer.isActive ? 1.0 : 0.0

                        Behavior on opacity { NumberAnimation { duration: 500 } }
                    }

                    Label {
                        id: itemText
                        text: name
                        color: buttonContainer.isActive ? Colors.highlightedText : Colors.light
                        font.bold: true
                        anchors {
                            left: parent.left
                            right: parent.right
                            verticalCenter: parent.verticalCenter
                            margins: Globals.defaultMargin
                        }
                        wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                    }
                }

                Repeater {
                    id: tags

                    property var library: App.libraries[index]

                    model: library.tags
                    delegate: tag

                    Component {
                        id: tag

                        ButtonContainer {
                            id: tagButtonContainer

                            property bool isActive: (currentLibrary === tags.library) &&
                                                    (currentTag === tags.library.tags[index])

                            mainItem: tagText
                            width: parent.width
                            cursorShape: Qt.PointingHandCursor

                            onClicked: {
                                if (currentLibrary !== tags.library) {
                                    sidebar.currentLibrary = tags.library;
                                }
                                if (sidebar.currentTag !== tags.library.tags[index]) {
                                    sidebar.currentTag = tags.library.tags[index];
                                }
                                libraryClicked(tags.library);
                                sidebar.close();
                            }

                            Rectangle {
                                anchors.fill: parent
                                color: Colors.highlight
                                visible: opacity > 0.0
                                opacity: tagButtonContainer.isActive ? 1.0 : 0.0

                                Behavior on opacity { NumberAnimation { duration: 500 } }
                            }

                            Label {
                                id: tagText
                                text: tags.library.tags[index]
                                color: tagButtonContainer.isActive ? Colors.highlightedText : Colors.light
                                anchors {
                                    left: parent.left
                                    right: parent.right
                                    verticalCenter: parent.verticalCenter
                                    margins: Globals.defaultMargin
                                    leftMargin: Globals.defaultMargin * 2
                                }
                                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                            }
                        }
                    }
                }
            }
        }

        Column {
            width: scrollView.width

            ButtonContainer {
                width: parent.width
                opacity: 0.5

                Item {
                    anchors {
                        left: parent.left
                        right: parent.right
                        margins: Globals.defaultMargin
                        verticalCenter: parent.verticalCenter
                    }

                    height: Math.max(addLocalLibrarySymbol.height, addLocalLibraryLabel.height)

                    Symbol {
                        id: addLocalLibrarySymbol
                        symbol: Fonts.symbols.faPlus
                        color: Colors.light
                        anchors {
                            left: parent.left
                            verticalCenter: parent.verticalCenter
                        }
                    }
                    Label {
                        id: addLocalLibraryLabel
                        text: qsTr("Add Library")
                        color: Colors.light
                        wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                        anchors {
                            left: addLocalLibrarySymbol.right
                            right: parent.right
                            verticalCenter: parent.verticalCenter
                        }
                    }
                }
                onClicked: {
                    sidebar.close();
                    sidebar.newLibrary();
                }
            }

            Repeater {
                id: librariesView
                model: App.libraries
                delegate: libraryItemDelegate
                anchors.fill: parent
            }

            ButtonContainer {
                width: parent.width
                mainItem: showAboutItem

                Rectangle {
                    anchors.fill: parent
                    color: sidebar.helpVisible ? Colors.highlight : "transparent"
                    visible: opacity > 0.0
                    opacity: sidebar.helpVisible ? 1.0 : 0.0

                    Behavior on opacity { NumberAnimation { duration: 500 } }
                }

                Item {
                    id: showAboutItem

                    anchors {
                        left: parent.left
                        right: parent.right
                        margins: Globals.defaultMargin
                        verticalCenter: parent.verticalCenter
                    }

                    height: Math.max(helpSymbol.height, helpText.height) + Globals.defaultMargin

                    Symbol {
                        id: helpSymbol
                        symbol: Fonts.symbols.faInfo
                        color: sidebar.helpVisible ? Colors.highlightedText : Colors.light
                        anchors {
                            left: parent.left
                            verticalCenter: parent.verticalCenter
                        }
                    }
                    Label {
                        id: helpText
                        text: qsTr("About...")
                        color: sidebar.helpVisible ? Colors.highlightedText : Colors.light
                        wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                        anchors {
                            left: helpSymbol.right
                            right: parent.right
                            verticalCenter: parent.verticalCenter
                        }
                    }
                }
                onClicked: {
                    sidebar.aboutPageRequested();
                    sidebar.close();
                }
            }
        }
    }


}
