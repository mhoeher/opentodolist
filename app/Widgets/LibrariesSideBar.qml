import QtQuick 2.5
import Qt.labs.settings 1.0

import "../Components"
import "../Controls" as C
import "../Fonts"
import "../Utils"
import "../Pages" as Pages

import OpenTodoList 1.0 as OTL

C.Pane {
    id: sidebar

    property alias numberOfLibraries: librariesModel.count

    property bool compact: false

    property C.StackView stack: null

    signal newLibrary
    signal aboutPageRequested
    signal settingsPageRequested
    signal accountsPageRequested
    signal close
    signal showLibrary(var library, var attributes)
    signal showItem(var library, var item)
    signal showSchedule(var library)

    function showSettings() {
        sidebar.settingsPageRequested()
    }

    function showAccounts() {
        sidebar.accountsPageRequested()
    }

    clip: true
    padding: 0
    Component.onCompleted: sortFilterModel.update()

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

            header: Column {
                width: parent.width

                LibrarySideBarButton {
                    text: qsTr("Schedule")
                    symbol: Icons.mdiSchedule
                    highlighted: d.bottommostPage instanceof Pages.ScheduleViewPage
                                 && d.bottommostPage.library === null
                    onClicked: sidebar.showSchedule(null)
                }
            }

            footer: Column {
                width: parent.width

                LibrarySideBarButton {
                    text: qsTr("New Library")
                    symbol: Icons.mdiAdd
                    highlighted: d.bottommostPage instanceof Pages.NewLibraryPage
                    onClicked: sidebar.newLibrary()
                }

                LibrarySideBarButton {
                    text: qsTr("Accounts")
                    symbol: Icons.mdiAccountCircle
                    highlighted: d.bottommostPage instanceof Pages.AccountsPage
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
                    highlighted: d.bottommostPage instanceof Pages.AboutPage
                    onClicked: sidebar.aboutPageRequested()
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
    }

    SortFilterModel {
        id: sortFilterModel
        model: OTL.LibrariesModel {
            id: librariesModel

            cache: OTL.Application.cache
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

            Settings {
                id: perLibrarySettings

                category: "LibrariesSideBar/Libraries/" + library.uid

                property alias noTagsExpanded: noTagColumn.expanded
            }

            Settings {
                id: perLibraryTagsSettings

                category: "LibrariesSideBar/Libraries/" + library.uid + "/tags"
            }

            LibrarySideBarButton {
                text: library.name
                bold: true
                symbol: librarySection.collapsed
                        || !symbolIsClickable ? Icons.mdiKeyboardArrowRight : Icons.mdiKeyboardArrowDown
                leftColorSwatch.color: library.color
                symbolIsClickable: library.tags.length > 0
                                   || librarySection.scheduleEnabled
                rightSymbolIsClickable: true
                rightSymbolIsVisible: d.editingListEntries
                rightSymbol: Icons.mdiEdit
                highlighted: d.bottommostPage instanceof Pages.LibraryPage
                             && d.bottommostPage.tag === ""
                             && !d.bottommostPage.untaggedOnly
                             && d.bottommostPage.library.uid === library.uid
                onClicked: sidebar.showLibrary(library, {})
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
                highlighted: d.bottommostPage instanceof Pages.ScheduleViewPage
                             && d.bottommostPage.library.uid === library.uid
                leftColorSwatch.color: library.color
                onClicked: sidebar.showSchedule(library)
            }

            Repeater {
                model: library.tags
                delegate: Column {
                    id: tagColumn

                    property string tag: modelData
                    property bool expanded: perLibraryTagsSettings.value(
                                                modelData, false)

                    onExpandedChanged: perLibraryTagsSettings.setValue(
                                           modelData, expanded)

                    width: parent.width
                    visible: !librarySection.collapsed

                    LibrarySideBarButton {
                        indent: 1
                        text: modelData
                        symbol: Icons.mdiLabel
                        leftColorSwatch.color: library.color
                        symbolIsClickable: true
                        highlighted: d.bottommostPage instanceof Pages.LibraryPage
                                     && d.bottommostPage.library.uid === library.uid
                                     && d.bottommostPage.tag === modelData
                        onClicked: sidebar.showLibrary(library, {
                                                           "tag": modelData
                                                       })
                        onSymbolClicked: tagColumn.expanded = !tagColumn.expanded
                    }

                    Repeater {
                        model: OTL.ItemsSortFilterModel {
                            sourceModel: OTL.ItemsModel {
                                cache: tagColumn.expanded ? OTL.Application.cache : null
                                parentItem: library.uid
                                tag: tagColumn.tag
                            }
                            sortRole: OTL.ItemsModel.TitleRole
                        }
                        delegate: LibrarySideBarButton {
                            indent: 2
                            text: title
                            leftColorSwatch.color: library.color
                            highlighted: d.bottommostPage.item?.uid === object.uid
                            onClicked: sidebar.showItem(library, object)
                        }
                    }
                }
            }

            Column {
                id: noTagColumn

                property bool expanded: false

                width: parent.width
                visible: !librarySection.collapsed

                LibrarySideBarButton {
                    indent: 1
                    text: qsTr("Untagged")
                    symbol: Icons.mdiLabelOff
                    leftColorSwatch.color: library.color
                    symbolIsClickable: true
                    highlighted: d.bottommostPage instanceof Pages.LibraryPage
                                 && d.bottommostPage.library.uid === library.uid
                                 && d.bottommostPage.untaggedOnly
                    onClicked: sidebar.showLibrary(library, {
                                                       "untaggedOnly": true
                                                   })
                    onSymbolClicked: noTagColumn.expanded = !noTagColumn.expanded
                }

                Repeater {
                    model: OTL.ItemsSortFilterModel {
                        sourceModel: OTL.ItemsModel {
                            cache: noTagColumn.expanded ? OTL.Application.cache : null
                            parentItem: library.uid
                            untaggedOnly: true
                        }
                        sortRole: OTL.ItemsModel.TitleRole
                    }
                    delegate: LibrarySideBarButton {
                        indent: 2
                        text: title
                        leftColorSwatch.color: library.color
                        highlighted: d.bottommostPage.item?.uid === object.uid
                        onClicked: sidebar.showItem(library, object)
                    }
                }
            }
        }
    }

    Connections {
        target: sidebar

        function onNewLibrary() {
            sidebar.close()
        }

        function onAboutPageRequested() {
            sidebar.close()
        }

        function onSettingsPageRequested() {
            sidebar.close()
        }

        function onAccountsPageRequested() {
            sidebar.close()
        }

        function onShowLibrary() {
            sidebar.close()
        }

        function onShowSchedule() {
            sidebar.close()
        }
    }

    Connections {
        target: sidebar.stack

        function onCurrentItemChanged() {
            if (sidebar.stack.depth > 0) {
                d.bottommostPage = sidebar.stack.get(0)
            } else {
                d.bottommostPage = null
            }
        }
    }

    QtObject {
        id: d

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
        property C.Page bottommostPage: null

        function isSelectedLibrary(lib) {
            if (lib === null && sidebar.currentLibrary === null) {
                return true
            }
            return !!lib && !!sidebar.currentLibrary
                    && lib.uid === sidebar.currentLibrary.uid
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
            list.forEach(uid => {
                             if (uid !== library.uid.toString()) {
                                 result.push(uid)
                             }
                         })
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
}
