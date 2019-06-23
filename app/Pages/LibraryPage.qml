import QtQuick 2.0
import QtQuick.Layouts 1.1

import OpenTodoList 1.0 as OTL

import "../Menues"
import "../Components"
import "../Widgets"
import "../Windows"
import "../Fonts"
import "../Utils"


Page {
    id: page

    property OTL.Library library: null
    property string tag: ""

    signal itemClicked(OTL.TopLevelItem item)
    signal closePage()
    signal openPage(var component, var properties)

    function newNote() {
        newNoteBar.edit.forceActiveFocus();
        newNoteBar.edit.text = "";
    }

    function newTodoList() {
        newTodoListBar.edit.forceActiveFocus();
        newTodoListBar.edit.text = "";
    }

    function newImage() {
        openImageDialog.open();
    }

    function find() {
        filterBar.edit.forceActiveFocus()
    }

    function deleteItem() {
        deleteLibraryDialog.deleteLibrary(library);
    }

    function renameItem() {
        renameLibraryDialog.renameLibrary(library);
    }

    property bool syncRunning: {
        return library &&
                OTL.Application.directoriesWithRunningSync.indexOf(
                    library.directory) >= 0;
    }

    property Menu pageMenu: LibraryPageMenu {
        x: page.width
        library: page.library
        onOpenPage: page.openPage(component, properties)
    }

    clip: true
    title: library.name

    QtObject {
        id: d


        function createNote(library, edit, tags) {
            var properties = {
                "title": edit.displayText,
                "tags": tags
            };
            var result = OTL.Application.addNote(library, properties);
            edit.text = "";
            edit.focus = false;
            return result;
        }

        function createTodoList(library, edit, tags) {
            var properties = {
                "title": edit.displayText,
                "tags": tags
            }

            var result = OTL.Application.addTodoList(library, properties);
            edit.text = "";
            edit.focus = false;
            return result;
        }

        function numberOfColumns(page) {
            var minWidth = Math.max(defaultFontPixelSize, 5) * 30;
            var result = page.width / minWidth;
            result = Math.ceil(result);
            result = Math.max(result, 1);
            return result;
        }

        function sizeOfColumns(page, correction) {
            if (correction === undefined) {
                correction = 0;
            }
            return (page.width - correction) / numberOfColumns(page);
        }

    }

    RenameLibraryDialog { id: renameLibraryDialog }
    DeleteLibraryDialog { id: deleteLibraryDialog }
    DeleteItemDialog { id: deleteItemDialog }
    RenameItemDialog { id: renameItemDialog }

    Menu {
        id: itemContextMenu

        property OTL.TopLevelItem item: null
        property var color: item ? item.color : OTL.TopLevelItem.White

        ButtonGroup { id: colorButtons }

        MenuItem {
            text: qsTr("Red")
            checkable: true
            ButtonGroup.group: colorButtons
            checked: itemContextMenu.color === OTL.TopLevelItem.Red
            onTriggered: itemContextMenu.item.color = OTL.TopLevelItem.Red
        }
        MenuItem {
            text: qsTr("Green")
            checkable: true
            ButtonGroup.group: colorButtons
            checked: itemContextMenu.color === OTL.TopLevelItem.Green
            onTriggered: itemContextMenu.item.color = OTL.TopLevelItem.Green
        }
        MenuItem {
            text: qsTr("Blue")
            checkable: true
            ButtonGroup.group: colorButtons
            checked: itemContextMenu.color === OTL.TopLevelItem.Blue
            onTriggered: itemContextMenu.item.color = OTL.TopLevelItem.Blue
        }
        MenuItem {
            text: qsTr("Yellow")
            checkable: true
            ButtonGroup.group: colorButtons
            checked: itemContextMenu.color === OTL.TopLevelItem.Yellow
            onTriggered: itemContextMenu.item.color = OTL.TopLevelItem.Yellow
        }
        MenuItem {
            text: qsTr("Orange")
            checkable: true
            ButtonGroup.group: colorButtons
            checked: itemContextMenu.color === OTL.TopLevelItem.Orange
            onTriggered: itemContextMenu.item.color = OTL.TopLevelItem.Orange
        }
        MenuItem {
            text: qsTr("Lilac")
            checkable: true
            ButtonGroup.group: colorButtons
            checked: itemContextMenu.color === OTL.TopLevelItem.Lilac
            onTriggered: itemContextMenu.item.color = OTL.TopLevelItem.Lilac
        }
        MenuItem {
            text: qsTr("White")
            checkable: true
            ButtonGroup.group: colorButtons
            checked: itemContextMenu.color === OTL.TopLevelItem.White
            onTriggered: itemContextMenu.item.color = OTL.TopLevelItem.White
        }

        MenuSeparator {}

        MenuItem {
            text: qsTr("Rename")
            onTriggered: renameItemDialog.renameItem(itemContextMenu.item)
        }

        MenuItem {
            text: qsTr("Delete")
            onTriggered: deleteItemDialog.deleteItem(itemContextMenu.item)
        }
    }

    OpenImageDialog {
        id: openImageDialog

        onAccepted: {
            var filename = OTL.Application.urlToLocalFile(fileUrl);
            var tags = [];
            if (librariesSideBar.currentTag !== "") {
                tags = [librariesSideBar.currentTag];
            }
            var properties = {
                "title": OTL.Application.basename(filename),
                "image": filename,
                "tags": tags
            };

            var image = OTL.Application.addImage(library, properties);
            itemCreatedNotification.show(image);
        }
    }

    OTL.ItemsSortFilterModel {
        id: itemsModel
        sourceModel: OTL.ItemsModel {
            cache: OTL.Application.cache
            tag: page.tag
            searchString: filterBar.text
            parentItem: page.library.uid
        }
    }

    TextInputBar {
        id: newNoteBar
        placeholderText: qsTr("Note Title")
        onAccepted: {
            var tags = [];
            if (librariesSideBar.currentTag !== "") {
                tags = [librariesSideBar.currentTag];
            }
            var note = d.createNote(library, newNoteBar.edit, tags);
            itemCreatedNotification.show(note);
        }
    }

    TextInputBar {
        id: newTodoListBar
        placeholderText: qsTr("Todo List Title")
        onAccepted: {
            var tags = [];
            if (librariesSideBar.currentTag !== "") {
                tags = [librariesSideBar.currentTag];
            }
            var todoList = d.createTodoList(library, newTodoListBar.edit, tags);
            itemCreatedNotification.show(todoList);
        }
    }

    TextInputBar {
        id: filterBar
        placeholderText: qsTr("Search term 1, search term 2, ...")
        symbol: Icons.faTimes
        showWhenNonEmpty: true
        closeOnButtonClick: true
    }

    ScrollView {
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
                source: Qt.resolvedUrl("../Widgets/" + object.itemType +
                                       "Item.qml")

                onLoaded: {
                    item.libraryItem = Qt.binding(function() {
                        return object;
                    });
                    item.library = page.library;
                    item.model = itemsModel;
                    item.onClicked.connect(function() {
                        itemClicked(object);
                    });
                    item.onReleased.connect(function(mouse) {
                        switch (mouse.button) {
                        case Qt.RightButton:
                            itemContextMenu.item = object;
                            itemContextMenu.parent = gridItem;
                            itemContextMenu.x = mouse.x;
                            itemContextMenu.y = mouse.y;
                            itemContextMenu.open();
                            break;
                        default:
                            break;
                        }
                    });
                }
            }
        }
    }    

    BackgroundLabel {
        visible: itemsModel.count === 0
        text: Markdown.stylesheet +
              qsTr("Nothing here yet! Start by adding a " +
                   "<a href='#note'>note</a>, " +
                   "<a href='#todolist'>todo list</a> or " +
                   "<a href='#image'>image</a>.")
        onLinkActivated: {
            switch (link) {
            case "#note":
                page.newNote();
                break;
            case "#todolist":
                page.newTodoList();
                break;
            case "#image":
                page.newImage();
                break;
            default:
                break;
            }
        }
    }

    NewTopLevelItemButton {
        onNewImage: page.newImage()
        onNewNote: page.newNote()
        onNewTodoList: page.newTodoList()
    }

    SyncIndicatorBar {
        id: syncIndicatorBar
        library: page.library
    }

    LibrarySecretsMissingNotificationBar {
        library: page.library
        onOpenPage: page.openPage(component, properties)
    }

    SyncErrorNotificationBar {
        readonly property var syncErrors: {
            if (page.library) {
                return OTL.Application.syncErrors[
                        page.library.directory] || [];
            } else {
                return [];
            }
        }

        library: page.library
        onShowErrors: page.openPage(syncErrorPage,
                                    {
                                        errors: syncErrors
                                    })
    }

    Component {
        id: syncErrorPage

        SyncErrorViewPage {}
    }

    ItemCreatedNotification {
        id: itemCreatedNotification

        onOpen: itemClicked(item)
    }

}
