import QtQuick 2.5
import QtQuick.Controls 2.2

import OpenTodoList 1.0
import OpenTodoList.UI 1.0

Rectangle {
    id: sidebar

    property Library currentLibrary: App.libraries[0]
    property string currentTag: ""
    property string specialView: ""

    property bool helpVisible: false
    property bool compact: false

    signal newLibrary()
    signal aboutPageRequested()
    signal close()

    color: Colors.dark
    clip: true
    onNewLibrary: close()

    ScrollView {
        id: scrollView

        anchors.fill: parent

        Column {
            width: scrollView.width

            Repeater {
                model: App.libraries

                delegate: Column {
                    id: librarySection

                    property Library library: modelData

                    width: parent.width

                    LibrarySideBarButton {
                        indent: 1
                        text: modelData.name
                        bold: true
                        active: currentLibrary === librarySection.library &&
                                currentTag === "" && specialView === ""
                        onClicked: {
                            currentLibrary = modelData;
                            currentTag = "";
                            specialView = "";
                            helpVisible = false;
                            sidebar.close();
                        }
                    }

                    LibrarySideBarButton {
                        indent: 2
                        text: qsTr("Schedule")
                        symbol: Fonts.symbols.faClockO
                        active: currentLibrary === librarySection.library &&
                                currentTag === "" && specialView === "schedule"
                        onClicked: {
                            currentLibrary = modelData;
                            currentTag = "";
                            specialView = "schedule";
                            helpVisible = false;
                            sidebar.close();
                        }
                    }

                    Repeater {
                        model: modelData.tags
                        delegate: LibrarySideBarButton {
                            indent: 2
                            text: modelData
                            symbol: Fonts.symbols.faTag
                            active: currentLibrary === librarySection.library &&
                                    currentTag === modelData &&
                                    specialView === ""
                            onClicked: {
                                currentLibrary = librarySection.library;
                                currentTag = modelData;
                                specialView = "";
                                helpVisible = false;
                                sidebar.close();
                            }
                        }
                    }
                }
            }

            LibrarySideBarButton {
                text: qsTr("New Library")
                symbol: Fonts.symbols.faPlus
                onClicked: sidebar.newLibrary()
            }

            LibrarySideBarButton {
                text: qsTr("About...")
                symbol: Fonts.symbols.faInfo
                active: helpVisible
                onClicked: {
                    helpVisible = true;
                    sidebar.aboutPageRequested();
                }
            }
        }

        Connections {
            target: currentLibrary
            onLibraryDeleted: {
                currentLibrary = null;
                currentTag = "";
                specialView = "";
            }
        }
    }
}
