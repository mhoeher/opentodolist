import QtQuick 2.5

import "../Components"
import "../Fonts"

import OpenTodoList 1.0 as OTL

Pane {
    id: sidebar

    property OTL.Library currentLibrary: OTL.Application.libraries[0] || null
    property string currentTag: ""
    property string specialView: ""

    property bool helpVisible: false
    property bool compact: false

    signal newLibrary()
    signal aboutPageRequested()
    signal close()

    clip: true
    padding: 0
    onNewLibrary: close()

    ScrollView {
        id: scrollView

        anchors.fill: parent

        Column {
            width: scrollView.width

            Repeater {
                model: OTL.LibrariesModel {
                    cache: OTL.Application.cache
                }

                delegate: Column {
                    id: librarySection

                    Component.onCompleted: console.warn(library)

                    width: parent.width

                    LibrarySideBarButton {
                        indent: 1
                        text: library.name
                        bold: true
                        symbol: library.hasSynchronizer ?
                                    Icons.faCloud :
                                    Icons.faFolderOpen
                        highlighted: currentLibrary === librarySection.library &&
                                currentTag === "" && specialView === ""
                        onClicked: {
                            currentLibrary = library;
                            currentTag = "";
                            specialView = "";
                            helpVisible = false;
                            sidebar.close();
                        }
                    }

                    LibrarySideBarButton {
                        indent: 2
                        text: qsTr("Schedule")
                        symbol: Icons.faClock
                        highlighted: currentLibrary === librarySection.library &&
                                currentTag === "" && specialView === "schedule"
                        onClicked: {
                            currentLibrary = library;
                            currentTag = "";
                            specialView = "schedule";
                            helpVisible = false;
                            sidebar.close();
                        }
                    }

                    Repeater {
                        model: library.tags
                        delegate: LibrarySideBarButton {
                            indent: 2
                            text: modelData
                            symbol: Icons.faTag
                            highlighted: currentLibrary === librarySection.library &&
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
                symbol: Icons.faPlus
                onClicked: sidebar.newLibrary()
            }

            LibrarySideBarButton {
                text: qsTr("About...")
                symbol: Icons.faInfo
                highlighted: helpVisible
                onClicked: {
                    helpVisible = true;
                    sidebar.aboutPageRequested();
                    sidebar.close();
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
