import QtQuick 2.10
import QtQuick.Layouts 1.1
import QtCore
import "../Dialogs" as Dialogs

import OpenTodoList 1.0 as OTL

import "../Menues"
import "../Components"
import "../Widgets"
import "../Windows"
import "../Fonts"
import "../Utils"
import "../Controls" as C

C.Page {
    id: page

    property OTL.Library library: null
    property string tag: ""
    property bool untaggedOnly: false

    signal closePage
    signal openPage(var component, var properties)

    savePage: function () {
        let result = {}
        if (page.library) {
            result.library = OTL.Application.uuidToString(page.library.uid)
        }
        if (page.tag !== "") {
            result.tag = page.tag
        }
        result.untaggedOnly = page.untaggedOnly
        return result
    }

    restorePage: function (data) {
        let uid = data.library
        if (uid) {
            d.restoreLibraryUid = OTL.Application.uuidFromString(uid)
            OTL.Application.loadLibrary(d.restoreLibraryUid)
        }
        let tag = data.tag
        if (tag) {
            page.tag = tag
        }
        page.untaggedOnly = !!data.untaggedOnly
    }

    restoreUrl: Qt.resolvedUrl("./LibraryPage.qml")

    function newNote() {
        newNoteBar.edit.forceActiveFocus()
        newNoteBar.edit.text = ""
    }

    function newTodoList() {
        newTodoListBar.edit.forceActiveFocus()
        newTodoListBar.edit.text = ""
    }

    function newImage() {
        openImageDialog.open()
    }

    function find() {
        filterBar.edit.forceActiveFocus()
    }

    function deleteItem() {
        deleteLibraryDialog.deleteLibrary(library)
    }

    function renameItem() {
        renameLibraryDialog.renameLibrary(library)
    }

    function selectColor() {
        colorDialog.selectedColor = page.library.color
        colorDialog.open()
    }

    function sort() {
        sortByMenu.popup()
    }

    property bool syncRunning: {
        return library && OTL.Application.directoriesWithRunningSync.indexOf(
                    library.directory) >= 0
    }
    property int syncProgress: {
        let result = -1
        if (library) {
            result = OTL.Application.syncProgress[library.directory]
            if (result === undefined) {
                result = -1
            }
        }
        return result
    }

    clip: true
    title: library?.name ?? ""

    Settings {
        id: settings

        property string sortBy: "weight"

        category: "LibraryPage"
    }

    QtObject {
        id: d

        property var restoreLibraryUid

        function createNote(library, edit, tags) {
            var properties = {
                "title": edit.displayText,
                "tags": tags
            }
            var result = OTL.Application.addNote(library, properties)
            edit.text = ""
            edit.focus = false
            return result
        }

        function createTodoList(library, edit, tags) {
            var properties = {
                "title": edit.displayText,
                "tags": tags
            }

            var result = OTL.Application.addTodoList(library, properties)
            edit.text = ""
            edit.focus = false
            return result
        }

        function numberOfColumns(page) {
            var minWidth = Math.max(
                        AppSettings.effectiveFontMetrics.averageCharacterWidth,
                        5) * AppSettings.libraryItemWidthScaleFactor
            var result = page.width / minWidth
            result = Math.ceil(result)
            result = Math.max(result, 1)
            return result
        }

        function sizeOfColumns(page, correction) {
            if (correction === undefined) {
                correction = 0
            }
            return (page.width - correction) / numberOfColumns(page)
        }

        function openItem(item) {
            page.C.StackView.view.push(Qt.resolvedUrl(
                                           "./" + item.itemType + "Page.qml"), {
                                           "item": OTL.Application.cloneItem(
                                                       item),
                                           "library": page.library
                                       })
        }
    }

    RenameLibraryDialog {
        id: renameLibraryDialog
    }
    DeleteLibraryDialog {
        id: deleteLibraryDialog
    }
    DeleteItemDialog {
        id: deleteItemDialog
    }
    RenameItemDialog {
        id: renameItemDialog
    }

    C.Menu {
        id: itemContextMenu

        property OTL.TopLevelItem item: null
        property var color: item ? item.color : OTL.TopLevelItem.White

        modal: true

        C.ButtonGroup {
            id: colorButtons
        }

        C.MenuItem {
            text: qsTr("Red")
            checkable: true
            C.ButtonGroup.group: colorButtons
            checked: itemContextMenu.color === OTL.TopLevelItem.Red
            onTriggered: itemContextMenu.item.color = OTL.TopLevelItem.Red
        }
        C.MenuItem {
            text: qsTr("Green")
            checkable: true
            C.ButtonGroup.group: colorButtons
            checked: itemContextMenu.color === OTL.TopLevelItem.Green
            onTriggered: itemContextMenu.item.color = OTL.TopLevelItem.Green
        }
        C.MenuItem {
            text: qsTr("Blue")
            checkable: true
            C.ButtonGroup.group: colorButtons
            checked: itemContextMenu.color === OTL.TopLevelItem.Blue
            onTriggered: itemContextMenu.item.color = OTL.TopLevelItem.Blue
        }
        C.MenuItem {
            text: qsTr("Yellow")
            checkable: true
            C.ButtonGroup.group: colorButtons
            checked: itemContextMenu.color === OTL.TopLevelItem.Yellow
            onTriggered: itemContextMenu.item.color = OTL.TopLevelItem.Yellow
        }
        C.MenuItem {
            text: qsTr("Orange")
            checkable: true
            C.ButtonGroup.group: colorButtons
            checked: itemContextMenu.color === OTL.TopLevelItem.Orange
            onTriggered: itemContextMenu.item.color = OTL.TopLevelItem.Orange
        }
        C.MenuItem {
            text: qsTr("Lilac")
            checkable: true
            C.ButtonGroup.group: colorButtons
            checked: itemContextMenu.color === OTL.TopLevelItem.Lilac
            onTriggered: itemContextMenu.item.color = OTL.TopLevelItem.Lilac
        }
        C.MenuItem {
            text: qsTr("White")
            checkable: true
            C.ButtonGroup.group: colorButtons
            checked: itemContextMenu.color === OTL.TopLevelItem.White
            onTriggered: itemContextMenu.item.color = OTL.TopLevelItem.White
        }

        C.MenuSeparator {}

        C.MenuItem {
            text: qsTr("Rename")
            onTriggered: renameItemDialog.renameItem(itemContextMenu.item)
        }

        C.MenuItem {
            text: qsTr("Copy")
            onTriggered: ItemUtils.copyTopLevelItem(itemContextMenu.item)
        }

        C.MenuItem {
            text: qsTr("Delete")
            onTriggered: deleteItemDialog.deleteItem(itemContextMenu.item)
        }
    }

    Dialogs.FileDialog {
        id: openImageDialog

        currentFolder: {
            let photosLocation = OTL.Application.getPhotoLibraryLocation()
            return photosLocation
        }
        title: qsTr("Select Image")
        nameFilters: ["Image Files (*.png *.bmp *.jpg *.jpeg *.gif)"]

        onAccepted: {
            var filename = OTL.Application.urlToLocalFile(selectedFile)
            var tags = []
            if (page.tag !== "") {
                tags = [page.tag]
            }
            var properties = {
                "title": OTL.Application.basename(filename),
                "imageUrl": selectedFile,
                "tags": tags
            }

            var image = OTL.Application.addImage(library, properties)
            itemCreatedNotification.show(image)
        }
    }

    ColorSelectionDialog {
        id: colorDialog

        onAccepted: if (selectedColor) {
                        page.library.color = selectedColor
                    } else {
                        page.library.resetColor()
                    }
    }

    OTL.ItemsSortFilterModel {
        id: itemsModel


        /**
         * @brief The role to sort by.
         *
         * This is a helper property, which is required in order to
         * get notified when the sort role changes (this is not the case
         * with the default sortRole property of QSortFilterProxyModel).
         */
        readonly property int effectiveSortRole: {
            switch (settings.sortBy) {
            case "dueTo":
                return OTL.ItemsModel.EffectiveDueToRole
            case "title":
                return OTL.ItemsModel.TitleRole
            case "createdAt":
                return OTL.ItemsModel.CreatedAtRole
            case "updatedAt":
                return OTL.ItemsModel.EffectiveUpdatedAtRole

                // By default, order manually:
            default:
                return OTL.ItemsModel.WeightRole
            }
        }

        sourceModel: OTL.ItemsModel {
            cache: OTL.Application.cache
            tag: page.tag
            untaggedOnly: page.untaggedOnly
            searchString: filterBar.text
            parentItem: page.library?.uid ?? ""
        }
        sortRole: effectiveSortRole
    }

    TextInputBar {
        id: newNoteBar
        placeholderText: qsTr("Note Title")
        onAccepted: {
            var tags = []
            if (page.tag !== "") {
                tags = [page.tag]
            }
            var note = d.createNote(library, newNoteBar.edit, tags)
            itemCreatedNotification.show(note)
        }
    }

    TextInputBar {
        id: newTodoListBar
        placeholderText: qsTr("Todo List Title")
        onAccepted: {
            var tags = []
            if (page.tag !== "") {
                tags = [page.tag]
            }
            var todoList = d.createTodoList(library, newTodoListBar.edit, tags)
            itemCreatedNotification.show(todoList)
        }
    }

    TextInputBar {
        id: filterBar
        placeholderText: qsTr("Search term 1, search term 2, ...")
        symbol: Icons.mdiClose
        showWhenNonEmpty: true
        closeOnButtonClick: true
    }

    C.ScrollView {
        id: scrollView
        anchors {
            left: parent.left
            right: parent.right
            bottom: parent.bottom
            top: filterBar.bottom
        }

        GridView {
            id: grid
            width: scrollView.width
            flow: GridView.LeftToRight
            model: itemsModel
            cellWidth: d.sizeOfColumns(scrollView)
            cellHeight: cellWidth / 3 * 2

            delegate: Loader {
                id: gridItem

                asynchronous: true
                width: grid.cellWidth
                height: grid.cellHeight
                source: Qt.resolvedUrl(
                            "../Widgets/" + object.itemType + "Item.qml")
                GridView.delayRemove: item ? item.GridView.delayRemove : false

                onLoaded: {
                    item.allowReordering = Qt.binding(function () {
                        return itemsModel.effectiveSortRole === OTL.ItemsModel.WeightRole
                    })
                    item.libraryItem = Qt.binding(function () {
                        return object
                    })
                    item.library = page.library
                    item.model = itemsModel
                    item.onClicked.connect(function (mouse) {
                        switch (mouse.button) {
                        case Qt.LeftButton:
                            d.openItem(object)
                            break
                        case Qt.RightButton:
                            itemContextMenu.item = object
                            itemContextMenu.parent = gridItem
                            itemContextMenu.x = mouse.x
                            itemContextMenu.y = mouse.y
                            itemContextMenu.open()
                            break
                        default:
                            break
                        }
                    })
                }
            }
        }
    }

    PullToRefreshOverlay {
        anchors.fill: scrollView
        refreshEnabled: page.library?.hasSynchronizer ?? false
        flickable: grid
        onRefresh: OTL.Application.syncLibrary(page.library)
    }

    AutoScrollOverlay {
        anchors.fill: parent
        flickable: grid
    }

    BackgroundLabel {
        visible: itemsModel.count === 0
        text: Markdown.stylesheet + qsTr(
                  "Nothing here yet! Start by adding a " + "<a href='#note'>note</a>, "
                  + "<a href='#todolist'>todo list</a> or " + "<a href='#image'>image</a>.")
        onLinkActivated: {
            switch (link) {
            case "#note":
                page.newNote()
                break
            case "#todolist":
                page.newTodoList()
                break
            case "#image":
                page.newImage()
                break
            default:
                break
            }
        }
    }

    NewTopLevelItemButton {
        onNewImage: page.newImage()
        onNewNote: page.newNote()
        onNewTodoList: page.newTodoList()
    }

    SyncErrorNotificationBar {
        readonly property var syncErrors: {
            if (page.library) {
                return OTL.Application.syncErrors[page.library.directory] || []
            } else {
                return []
            }
        }

        library: page.library
        onShowErrors: page.openPage(syncErrorPage, {
                                        "errors": syncErrors
                                    })
    }

    Component {
        id: syncErrorPage

        SyncErrorViewPage {}
    }

    ItemCreatedNotification {
        id: itemCreatedNotification

        onOpen: d.openItem(item)
    }

    C.Menu {
        id: sortByMenu

        title: qsTr("Sort By")
        modal: true

        C.MenuItem {
            text: qsTr("Manually")
            checkable: true
            checked: itemsModel.effectiveSortRole === OTL.ItemsModel.WeightRole
            onTriggered: settings.sortBy = "weight"
        }

        C.MenuItem {
            text: qsTr("Title")
            checkable: true
            checked: itemsModel.effectiveSortRole === OTL.ItemsModel.TitleRole
            onTriggered: settings.sortBy = "title"
        }

        C.MenuItem {
            text: qsTr("Due To")
            checkable: true
            checked: itemsModel.effectiveSortRole === OTL.ItemsModel.EffectiveDueToRole
            onTriggered: settings.sortBy = "dueTo"
        }

        C.MenuItem {
            text: qsTr("Created At")
            checkable: true
            checked: itemsModel.effectiveSortRole === OTL.ItemsModel.CreatedAtRole
            onTriggered: settings.sortBy = "createdAt"
        }

        C.MenuItem {
            text: qsTr("Updated At")
            checkable: true
            checked: itemsModel.effectiveSortRole === OTL.ItemsModel.EffectiveUpdatedAtRole
            onTriggered: settings.sortBy = "updatedAt"
        }
    }

    Connections {
        target: OTL.Application

        function onLibraryLoaded(uid, data) {
            if (uid === d.restoreLibraryUid) {
                page.library = OTL.Application.libraryFromData(data)
            }
        }
    }
}
