import QtQuick 2.5
import Qt.labs.settings 1.0

import "../Components"
import "../Controls" as C
import "../Fonts"
import "../Utils"

import OpenTodoList 1.0 as OTL

C.Pane {
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

    signal newLibrary
    signal aboutPageRequested
    signal settingsPageRequested
    signal accountsPageRequested
    signal close
    signal showLibrary(var library, string tag, string specialView)

    function doShowLibrary() {
        showLibrary(currentLibrary, currentTag, specialView)
    }

    function reopenLastLibrary() {
        if (lastLibrary != "" && !previousLibraryOpened) {
            for (var i = 0; i < librariesModel.count; ++i) {
                var lib = librariesModel.get(i)
                if (lib.uid.toString() === lastLibrary) {
                    currentLibrary = lib
                    currentTag = lastTag
                    specialView = lastSpecialView
                    doShowLibrary()
                }
            }
        }
        previousLibraryOpened = true
    }

    function showSettings() {
        settingsVisible = true
        helpVisible = false
        accountsVisible = false
        sidebar.settingsPageRequested()
        sidebar.close()
    }

    function showAccounts() {
        accountsVisible = true
        helpVisible = false
        settingsVisible = false
        sidebar.accountsPageRequested()
        sidebar.close()
    }

    clip: true
    padding: 0
    onNewLibrary: close()
    Component.onCompleted: sortFilterModel.update()

    QtObject {
        id: d

        function isSelectedLibrary(lib) {
            return !!lib && !!sidebar.currentLibrary
                    && lib.uid === sidebar.currentLibrary.uid
        }

        property bool editingListEntries: false
        property var collapsedLibraries: []
        property var librariesWithoutSchedule: []
        property var libraryOrder: []
        readonly property var libraryWeights: {
            let result = {}
            for (var i = 0; i < libraryOrder.length; ++i) {
                result[libraryOrder[i]] = i
            }
            return result
        }

        function collapse(library) {
            collapsedLibraries = addLibraryIdToListOfUids(collapsedLibraries,
                                                          library)
        }

        function expand(library) {
            collapsedLibraries = removeLibraryIdFromListOfUids(
                        collapsedLibraries, library)
        }

        function showScheduleForLibrary(library) {
            librariesWithoutSchedule = removeLibraryIdFromListOfUids(
                        librariesWithoutSchedule, library)
        }

        function hideScheduleForLibrary(library) {
            librariesWithoutSchedule = addLibraryIdToListOfUids(
                        librariesWithoutSchedule, library)
        }

        function addLibraryIdToListOfUids(list, library) {
            let uids = removeLibraryIdFromListOfUids(list, library)
            uids.push(library.uid.toString())
            return uids
        }

        function removeLibraryIdFromListOfUids(list, library) {
            let result = []
            for (const uid in list) {
                if (uid !== library.uid.toString()) {
                    result.push(uid)
                }
            }
            return result
        }

        function moveLibraryToIndex(library, index) {
            // Get current order:
            let order = []
            let currentIndex = -1
            for (var i = 0; i < sortFilterModel.count; ++i) {
                let lib = sortFilterModel.get(i).library
                order.push(lib.uid.toString())
                if (lib === library) {
                    currentIndex = i
                }
            }

            // Move library to desired index:
            order = d.removeLibraryIdFromListOfUids(order, library)
            order.splice(index, 0, library.uid.toString())
            d.libraryOrder = order
        }

        function getLibraryCurrentIndex(library) {
            for (var i = 0; i < sortFilterModel.count; ++i) {
                let lib = sortFilterModel.get(i).library
                if (lib === library) {
                    return i
                }
            }
            return -1
        }

        // Update the sort/filter model on changes:
        onLibraryWeightsChanged: sortFilterModel.update()
    }

    Settings {
        category: "LibrariesSideBar"
        property alias collapsedLibraries: d.collapsedLibraries
        property alias librariesWithoutSchedule: d.librariesWithoutSchedule
        property alias libraryOrder: d.libraryOrder
    }

    C.ScrollView {
        id: scrollView

        anchors.fill: parent

        ListView {
            width: scrollView.availableWidth
            model: sortFilterModel
            implicitWidth: childrenRect.width
            implicitHeight: childrenRect.height

            footer: Column {
                width: parent.width

                LibrarySideBarButton {
                    text: qsTr("New Library")
                    symbol: Icons.mdiAdd
                    onClicked: sidebar.newLibrary()
                }

                LibrarySideBarButton {
                    text: qsTr("Accounts")
                    symbol: Icons.mdiAccountCircle
                    highlighted: accountsVisible
                    onClicked: sidebar.showAccounts()
                }

                LibrarySideBarButton {
                    text: qsTr("Edit List")
                    symbol: d.editingListEntries ? Icons.mdiVisibility : Icons.mdiVisibilityOff
                    onClicked: d.editingListEntries = !d.editingListEntries
                }

                LibrarySideBarButton {
                    text: qsTr("Settings")
                    symbol: Icons.mdiSettings
                    highlighted: settingsVisible
                    onClicked: sidebar.showSettings()
                }

                LibrarySideBarButton {
                    text: qsTr("Translate The App...")
                    symbol: Icons.mdiTranslate
                    onClicked: Qt.openUrlExternally(
                                   "https://poeditor.com/join/project/ztvOymGNxn")
                }

                LibrarySideBarButton {
                    text: qsTr("Donate")
                    symbol: Icons.mdiSavings
                    visible: Qt.platform.os != "ios"
                    onClicked: Qt.openUrlExternally(
                                   "https://opentodolist.rpdev.net/donate/")
                }

                LibrarySideBarButton {
                    text: qsTr("About...")
                    symbol: Icons.mdiInfo
                    highlighted: helpVisible
                    onClicked: {
                        helpVisible = true
                        settingsVisible = false
                        sidebar.aboutPageRequested()
                        sidebar.close()
                    }
                }

                LibrarySideBarButton {
                    visible: isDebugBuild
                    text: qsTr("Create Default Library")
                    onClicked: {
                        var lib = OTL.Application.addLocalLibrary("My Library")

                        var note = OTL.Application.addNote(lib, {})
                        note.title = "A Note"
                        note.notes = "* This is a note\n* It stores arbitrary text"

                        var todoList = OTL.Application.addTodoList(lib, {})
                        todoList.title = "A Todo List"
                        todoList.notes = "* Todo lists contain todos.\n"
                                + "* Todos in turn can contain tasks."

                        var todo1 = OTL.Application.addTodo(lib, todoList, {})
                        todo1.title = "A todo"

                        var todo2 = OTL.Application.addTodo(lib, todoList, {})
                        todo2.title = "Another Todo"

                        var image = OTL.Application.addImage(lib, {})
                        image.title = "An Image"
                        image.image = ":/sample.png"
                    }
                }
            }
        }

        Connections {
            target: currentLibrary
            function onLibraryDeleted() {
                currentLibrary = null
                currentTag = ""
                specialView = ""
                doShowLibrary()
            }
        }
    }

    SortFilterModel {
        id: sortFilterModel
        model: OTL.LibrariesModel {
            id: librariesModel

            cache: OTL.Application.cache
            onUpdateFinished: sidebar.reopenLastLibrary()
        }
        lessThan: function (left, right) {
            let weights = d.libraryWeights
            let leftLib = left.library
            let rightLib = right.library
            let leftWeight = d.libraryWeights[leftLib.uid.toString()]
            let rightWeight = d.libraryWeights[rightLib.uid.toString()]
            return leftWeight < rightWeight
        }
        delegate: Column {
            id: librarySection

            readonly property bool collapsed: {
                for (var i = 0; i < d.collapsedLibraries.length; ++i) {
                    if (d.collapsedLibraries[i] === library.uid.toString()) {
                        return true
                    }
                }
                return false
            }

            readonly property bool scheduleEnabled: {
                for (const libId in d.librariesWithoutSchedule) {
                    if (libId === library.uid.toString()) {
                        return false
                    }
                }
                return true
            }

            width: parent.width

            LibrarySideBarButton {
                text: library.name
                bold: true
                symbol: librarySection.collapsed
                        || !symbolIsClickable ? Icons.mdiKeyboardArrowRight : Icons.mdiKeyboardArrowDown
                highlighted: d.isSelectedLibrary(library) && currentTag === ""
                             && specialView === ""
                symbolIsClickable: library.tags.length > 0
                                   || librarySection.scheduleEnabled
                rightSymbolIsClickable: true
                rightSymbolIsVisible: d.editingListEntries
                rightSymbol: Icons.mdiEdit
                onClicked: {
                    currentLibrary = library
                    currentTag = ""
                    specialView = ""
                    helpVisible = false
                    settingsVisible = false
                    sidebar.close()
                    doShowLibrary()
                }
                onSymbolClicked: {
                    if (librarySection.collapsed) {
                        d.expand(library)
                    } else {
                        d.collapse(library)
                    }
                }
                onRightSymbolClicked: libraryContextMenu.open()

                C.Menu {
                    id: libraryContextMenu

                    modal: true
                    C.MenuItem {
                        text: librarySection.scheduleEnabled ? qsTr("Hide Schedule") : qsTr(
                                                                   "Show Schedule")
                        onTriggered: {
                            if (librarySection.scheduleEnabled) {
                                d.hideScheduleForLibrary(library)
                            } else {
                                d.showScheduleForLibrary(library)
                            }
                        }
                    }
                    C.MenuItem {
                        text: qsTr("Move Up")
                        onTriggered: {
                            let currentIndex = d.getLibraryCurrentIndex(library)
                            if (currentIndex > 0) {
                                d.moveLibraryToIndex(library, currentIndex - 1)
                            }
                        }
                    }
                    C.MenuItem {
                        text: qsTr("Move Down")
                        onTriggered: {
                            let currentIndex = d.getLibraryCurrentIndex(library)
                            if (currentIndex < sortFilterModel.count - 1) {
                                d.moveLibraryToIndex(library, currentIndex + 1)
                            }
                        }
                    }
                }
            }

            LibrarySideBarButton {
                indent: 1
                text: qsTr("Schedule")
                symbol: Icons.mdiSchedule
                visible: !librarySection.collapsed
                         && librarySection.scheduleEnabled
                highlighted: d.isSelectedLibrary(library) && currentTag === ""
                             && specialView === "schedule"
                onClicked: {
                    currentLibrary = library
                    currentTag = ""
                    specialView = "schedule"
                    helpVisible = false
                    settingsVisible = false
                    sidebar.close()
                    doShowLibrary()
                }
            }

            Repeater {
                model: library.tags
                delegate: LibrarySideBarButton {
                    indent: 1
                    visible: !librarySection.collapsed
                    text: modelData
                    symbol: Icons.mdiLabel
                    highlighted: d.isSelectedLibrary(library)
                                 && currentTag === modelData
                                 && specialView === ""
                    onClicked: {
                        currentLibrary = library
                        currentTag = modelData
                        specialView = ""
                        helpVisible = false
                        settingsVisible = false
                        sidebar.close()
                        doShowLibrary()
                    }
                }
            }
        }
    }
}
