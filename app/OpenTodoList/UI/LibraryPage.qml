import QtQuick 2.0
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.2
import QtQuick.Controls.Material 2.2

import OpenTodoList 1.0
import OpenTodoList.UI 1.0

import "LibraryPageLogic.js" as Logic

Page {
    id: page

    property Library library: null
    property string tag: ""

    signal itemClicked(TopLevelItem item)
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

    property var deleteItem: library === App.defaultLibrary ? null :
                                                              function() {
                                                                  confirmDeleteLibrary.open();
                                                              }
    property bool syncRunning: library && library.synchronizing
    property Menu pageMenu: Menu {
        x: page.width

        MenuItem {
            text: qsTr("Edit Sync Settings")
            enabled: page.library.hasSynchronizer
            onClicked: {
                var sync = page.library.createSynchronizer();
                if (sync !== null) {
                    var key = sync.secretsKey;
                    if (key !== "") {
                        sync.secret = App.secretForSynchronizer(sync);
                    }
                    var url = Qt.resolvedUrl(sync.type + "SettingsPage.qml");
                    page.openPage(url, {"synchronizer": sync});
                }
            }
        }

        MenuItem {
            text: qsTr("Sync Now")
            enabled: page.library.hasSynchronizer
            onClicked: {
                console.debug("Manually started syncing " + page.library.name);
                App.syncLibrary(library);
            }
        }

        MenuSeparator {}

        MenuItem {
            text: qsTr("Sync Log")
            enabled: page.library.hasSynchronizer
            onClicked: openPage(logPage, {"log": page.library.syncLog()})
        }
    }

    clip: true

    CenteredDialog {
        id: confirmDeleteLibrary
        title: qsTr("Delete Library?")
        Label {
            property string textDefaultLocation: {
                return qsTr("Do you really want to remove the library " +
                            "<strong>%1</strong> from  the " +
                            "application? <em>This will remove any files " +
                            "belonging to the library.</em>"
                            )
                .arg(library.name);
            }
            property string textNonDefaultLocation: {
                return qsTr("Do you really want to remove the library " +
                            "<strong>%1</strong> from the " +
                            "application? Note that the files inside the " +
                            "library will not be removed, so " +
                            "you can restore the library later on.")
                .arg(library.name);
            }

            text: library.isInDefaultLocation ? textDefaultLocation : textNonDefaultLocation
            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
            anchors {
                left: parent.left
                right: parent.right
                top: parent.top
                margins: Globals.defaultMargin
            }
        }
        standardButtons: Dialog.Ok | Dialog.Cancel
        onAccepted: {
            library.deleteLibrary(library.isInDefaultLocation);
        }
    }

    CenteredDialog {
        id: confirmDeleteItem
        title: qsTr("Delete Item?")
        Label {
            text: qsTr("Do you really want to delete the item <strong>%1</strong> from the library? " +
                       "This action cannot be undone!").arg(
                      itemContextMenu.item ? itemContextMenu.item.displayTitle : "")
            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
            anchors {
                left: parent.left
                right: parent.right
                top: parent.top
                margins: Globals.defaultMargin
            }
        }
        standardButtons:Dialog.Ok | Dialog.Cancel
        onAccepted: itemContextMenu.item.deleteItem()
    }

    RenameItemDialog {
        id: renameItemDialog
    }

    Menu {
        id: itemContextMenu

        property TopLevelItem item: null
        property var color: item ? item.color : TopLevelItem.White

        ButtonGroup { id: colorButtons }

        MenuItem {
            text: qsTr("Red")
            checkable: true
            ButtonGroup.group: colorButtons
            checked: itemContextMenu.color === TopLevelItem.Red
            onTriggered: itemContextMenu.item.color = TopLevelItem.Red
        }
        MenuItem {
            text: qsTr("Green")
            checkable: true
            ButtonGroup.group: colorButtons
            checked: itemContextMenu.color === TopLevelItem.Green
            onTriggered: itemContextMenu.item.color = TopLevelItem.Green
        }
        MenuItem {
            text: qsTr("Blue")
            checkable: true
            ButtonGroup.group: colorButtons
            checked: itemContextMenu.color === TopLevelItem.Blue
            onTriggered: itemContextMenu.item.color = TopLevelItem.Blue
        }
        MenuItem {
            text: qsTr("Yellow")
            checkable: true
            ButtonGroup.group: colorButtons
            checked: itemContextMenu.color === TopLevelItem.Yellow
            onTriggered: itemContextMenu.item.color = TopLevelItem.Yellow
        }
        MenuItem {
            text: qsTr("Orange")
            checkable: true
            ButtonGroup.group: colorButtons
            checked: itemContextMenu.color === TopLevelItem.Orange
            onTriggered: itemContextMenu.item.color = TopLevelItem.Orange
        }
        MenuItem {
            text: qsTr("Lilac")
            checkable: true
            ButtonGroup.group: colorButtons
            checked: itemContextMenu.color === TopLevelItem.Lilac
            onTriggered: itemContextMenu.item.color = TopLevelItem.Lilac
        }
        MenuItem {
            text: qsTr("White")
            checkable: true
            ButtonGroup.group: colorButtons
            checked: itemContextMenu.color === TopLevelItem.White
            onTriggered: itemContextMenu.item.color = TopLevelItem.White
        }

        MenuSeparator {}

        MenuItem {
            text: qsTr("Rename")
            onTriggered: renameItemDialog.renameItem(itemContextMenu.item)
        }

        MenuItem {
            text: qsTr("Delete")
            onTriggered: confirmDeleteItem.open()
        }
    }

    OpenImageDialog {
        id: openImageDialog

        onAccepted: {
            var image = library.addImage();
            var filename = App.urlToLocalFile(fileUrl);
            image.image = filename;
            image.title = App.basename(filename);
            if (leftSideBar.currentTag !== "") {
                image.addTag(leftSideBar.currentTag);
            }
            itemCreatedNotification.show(image);
        }
    }

    ItemsSortFilterModel {
        id: itemsModel
        sourceModel: ItemsModel {
            container: page.library.topLevelItems
        }
    }

    ItemsSortFilterModel {
        id: filteredItemsModel

        sourceModel: itemsModel
        tag: page.tag
        searchString: filterBar.text
    }

    TextInputBar {
        id: newNoteBar
        placeholderText: qsTr("Note Title")
        onAccepted: {
            var note = Logic.createNote(library, newNoteBar.edit);
            if (leftSideBar.currentTag !== "") {
                note.addTag(leftSideBar.currentTag)
            }
            itemCreatedNotification.show(note);
        }
    }

    TextInputBar {
        id: newTodoListBar
        placeholderText: qsTr("Todo List Title")
        onAccepted: {
            var todoList = Logic.createTodoList(library, newTodoListBar.edit);
            if (leftSideBar.currentTag !== "") {
                todoList.addTag(leftSideBar.currentTag)
            }
            itemCreatedNotification.show(todoList);
        }
    }

    TextInputBar {
        id: filterBar
        placeholderText: qsTr("Search term 1, search term 2, ...")
        symbol: Fonts.symbols.faTimes
        color: Colors.mid
        showWhenNonEmpty: true
        closeOnButtonClick: true
    }

    BackgroundSymbol {
        symbol: page.tag !== "" ? Fonts.symbols.faTag : Fonts.symbols.faGrid
    }

    ScrollView {
        id: scrollView
        anchors {
            left: parent.left
            right: parent.right
            bottom: parent.bottom
            top: filterBar.bottom
            topMargin: filterBar.shown ? filterBar.contentHeight - filterBar.height : 0
        }

        GridView {
            id: grid
            width: scrollView.width
            flow: GridView.LeftToRight
            model: filteredItemsModel
            cellWidth: Logic.sizeOfColumns(scrollView)
            cellHeight: cellWidth / 3 * 2
            delegate: Loader {
                id: gridItem

                asynchronous: true
                width: grid.cellWidth
                height: grid.cellHeight
                source: Qt.resolvedUrl(object.itemType + "Item.qml")

                onLoaded: {
                    item.libraryItem = object;
                    item.library = page.library;
                    item.onClicked.connect(function() {

                    });
                    item.onReleased.connect(function(mouse) {
                        switch (mouse.button) {
                        case Qt.LeftButton:
                            itemClicked(object);
                            break;
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
        visible: filteredItemsModel.count === 0
        text: qsTr("Nothing here yet! Start by adding a " +
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

    Rectangle {
        id: syncIndicatorBar

        color: Colors.midlight
        border {
            width: 1
            color: Colors.mid
        }
        height: childrenRect.height
        width: parent.width
        y: parent.height
        state: page.library.synchronizing ? "visible" : ""

        states: State {
            name: "visible"

            PropertyChanges {
                target: syncIndicatorBar
                y: page.height - syncIndicatorBar.height
            }
        }

        transitions: [
            Transition {
                from: ""
                to: "visible"

                SmoothedAnimation {
                    properties: "y"
                }
            },
            Transition {
                from: "visible"
                to: ""

                SmoothedAnimation {
                    properties: "y"
                }
            }
        ]

        RowLayout {
            height: childrenRect.height
            anchors {
                left: parent.left
                right: parent.right
                margins: Globals.defaultMargin
            }

            Label {
                Layout.fillWidth: true
                text: qsTr("Synchronizing library...")
            }

            Label {
                font.family: Fonts.symbols.name
                text: Fonts.symbols.faSpinner

                NumberAnimation on rotation {
                    from: 0
                    to: 360
                    duration: 2000
                    loops: Animation.Infinite
                }
            }
        }
    }

    LibrarySecretsMissingNotificationBar { library: page.library }

    SyncErrorNotificationBar {
        library: page.library
        onShowErrors: page.openPage(syncErrorPage, { errors: page.library.syncErrors })
    }

    ItemCreatedNotification {
        id: itemCreatedNotification

        onOpen: itemClicked(item)
    }

    Component {
        id: logPage

        LogViewPage {}
    }

    Component {
        id: syncErrorPage

        SyncErrorViewPage {}
    }
}
