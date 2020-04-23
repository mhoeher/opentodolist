import QtQuick 2.5
import QtQuick.Controls 2.12
import Qt.labs.settings 1.0

import "../Components"
import "../Fonts"
import "../Utils"

import OpenTodoList 1.0 as OTL

Pane {
    id: sidebar

    property OTL.Library currentLibrary: null
    property string currentTag: ""
    property string specialView: ""
    property alias numberOfLibraries: librariesModel.count

    // For reopening the last view:
    property string lastLibrary: ""
    property string lastTag: ""
    property string lastSpecialView: ""
    property bool previousLibraryOpened: false

    property bool helpVisible: false
    property bool settingsVisible: false
    property bool accountsVisible: false
    property bool compact: false

    signal newLibrary()
    signal aboutPageRequested()
    signal settingsPageRequested()
    signal accountsPageRequested()
    signal close()

    function reopenLastLibrary() {
        if (lastLibrary != "" && !previousLibraryOpened) {
            for (var i = 0; i < librariesModel.count; ++i) {
                var lib = librariesModel.get(i);
                if (lib.uid.toString() === lastLibrary) {
                    currentLibrary = lib;
                    currentTag = lastTag;
                    specialView = lastSpecialView;
                }
            }
        }
        previousLibraryOpened = true;
    }

    function showSettings() {
        settingsVisible = true;
        helpVisible = false;
        accountsVisible = false;
        sidebar.settingsPageRequested();
        sidebar.close();
    }

    function showAccounts() {
        accountsVisible = true;
        helpVisible = false;
        settingsVisible = false;
        sidebar.accountsPageRequested();
        sidebar.close();
    }

    clip: true
    padding: 0
    onNewLibrary: close()

    QtObject {
        id: d

        function isSelectedLibrary(lib) {
            return !!lib && !!sidebar.currentLibrary &&
                    lib.uid === sidebar.currentLibrary.uid;
        }

        property var collapsedLibraries: []

        function collapse(library) {
            let ids = [];
            let append = true;
            for (let i = 0; i < d.collapsedLibraries.length; ++i) {
                let id = d.collapsedLibraries[i];
                if (id === library.uid.toString()) {
                    append = false;
                }
                ids.push(id);
            }
            if (append) {
                ids.push(library.uid.toString());
            }
            d.collapsedLibraries = ids;
        }

        function expand(library) {
            let ids = [];
            for (let i = 0; i < d.collapsedLibraries.length; ++i) {
                let id = d.collapsedLibraries[i];
                if (id !== library.uid.toString()) {
                    ids.push(id);
                }
            }
            d.collapsedLibraries = ids;
        }
    }

    Settings {
        category: "LibrariesSideBar"
        property alias collapsedLibraries: d.collapsedLibraries
    }

    ScrollView {
        id: scrollView

        anchors.fill: parent

        Column {
            width: scrollView.width

            Repeater {
                model: OTL.LibrariesModel {
                    id: librariesModel

                    cache: OTL.Application.cache
                    onUpdateFinished: sidebar.reopenLastLibrary()
                }

                delegate: Column {
                    id: librarySection

                    readonly property bool collapsed: {
                        for (var i = 0; i < d.collapsedLibraries.length; ++i) {
                            if (d.collapsedLibraries[i] === library.uid.toString()) {
                                return true;
                            }
                        }
                        return false;
                    }

                    width: parent.width

                    LibrarySideBarButton {
                        text: library.name
                        bold: true
                        symbol: librarySection.collapsed ?
                                    Icons.faChevronRight :
                                    Icons.faChevronDown
                        highlighted: d.isSelectedLibrary(library) &&
                                currentTag === "" && specialView === ""
                        symbolIsClickable: true
                        onClicked: {
                            currentLibrary = library;
                            currentTag = "";
                            specialView = "";
                            helpVisible = false;
                            settingsVisible = false;
                            sidebar.close();
                        }
                        onSymbolClicked: {
                            if (librarySection.collapsed) {
                                d.expand(library);
                            } else {
                                d.collapse(library);
                            }
                        }
                    }

                    LibrarySideBarButton {
                        indent: 1
                        text: qsTr("Schedule")
                        symbol: Icons.faClock
                        visible: !librarySection.collapsed
                        highlighted: d.isSelectedLibrary(library) &&
                                currentTag === "" && specialView === "schedule"
                        onClicked: {
                            currentLibrary = library;
                            currentTag = "";
                            specialView = "schedule";
                            helpVisible = false;
                            settingsVisible = false;
                            sidebar.close();
                        }
                    }

                    Repeater {
                        model: library.tags
                        delegate: LibrarySideBarButton {
                            indent: 1
                            visible: !librarySection.collapsed
                            text: modelData
                            symbol: Icons.faTag
                            highlighted: d.isSelectedLibrary(library) &&
                                    currentTag === modelData &&
                                    specialView === ""
                            onClicked: {
                                currentLibrary = library;
                                currentTag = modelData;
                                specialView = "";
                                helpVisible = false;
                                settingsVisible = false;
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
                text: qsTr("Accounts")
                symbol: Icons.faUser
                highlighted: accountsVisible
                onClicked: sidebar.showAccounts()
            }

            LibrarySideBarButton {
                text: qsTr("Settings")
                symbol: Icons.faWrench
                highlighted: settingsVisible
                onClicked: sidebar.showSettings()
            }

            LibrarySideBarButton {
                text: qsTr("Translate The App...")
                symbol: Icons.faLanguage
                onClicked: Qt.openUrlExternally(
                               "https://poeditor.com/join/project/ztvOymGNxn")
            }

            LibrarySideBarButton {
                text: qsTr("About...")
                symbol: Icons.faInfo
                highlighted: helpVisible
                onClicked: {
                    helpVisible = true;
                    settingsVisible = false;
                    sidebar.aboutPageRequested();
                    sidebar.close();
                }
            }

            LibrarySideBarButton {
                visible: isDebugBuild
                text: qsTr("Create Default Library")
                onClicked: {
                    var lib = OTL.Application.addLocalLibrary("My Library");

                    var note = OTL.Application.addNote(lib, {});
                    note.title = "A Note";
                    note.notes = "* This is a note\n* It stores arbitrary text";

                    var todoList = OTL.Application.addTodoList(lib, {});
                    todoList.title = "A Todo List";
                    todoList.notes = "* Todo lists contain todos.\n" +
                            "* Todos in turn can contain tasks.";

                    var todo1 = OTL.Application.addTodo(lib, todoList, {});
                    todo1.title = "A todo";

                    var todo2 = OTL.Application.addTodo(lib, todoList, {});
                    todo2.title = "Another Todo";

                    var image = OTL.Application.addImage(lib, {});
                    image.title = "An Image";
                    image.image = ":/sample.png";
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
