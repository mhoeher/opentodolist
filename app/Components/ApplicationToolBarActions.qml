import QtQuick 2.5

import "." as Components
import "../Menues" as Menues
import "../Fonts" as Fonts

Item {

    // The list of all available page actions
    property list<ToolBarAction> dynamicPageActions
    property Components.ApplicationShortcuts shortcuts: null

    dynamicPageActions: [
        ToolBarAction {
            symbol: Fonts.Icons.mdiUndo
            text: qsTr("Undo")
            visible: shortcuts.undoShortcut.enabled
            onTriggered: shortcuts.undoShortcut.trigger()
        },

        ToolBarAction {
            symbol: Fonts.Icons.mdiEdit
            text: qsTr("Rename")
            visible: {
                return stackView.currentItem
                        && typeof (stackView.currentItem["renameItem"]) === "function"
            }
            onTriggered: stackView.currentItem.renameItem()
        },

        ToolBarAction {
            symbol: Fonts.Icons.mdiContentCut
            text: qsTr("Move")
            visible: {
                return stackView.currentItem
                        && typeof (stackView.currentItem["moveItem"]) === "function"
            }
            onTriggered: stackView.currentItem.moveItem()
        },

        ToolBarAction {
            symbol: Fonts.Icons.mdiContentCopy
            text: qsTr("Copy")
            visible: {
                return stackView.currentItem
                        && typeof (stackView.currentItem["copyItem"]) === "function"
            }
            onTriggered: stackView.currentItem.copyItem()
        },

        ToolBarAction {
            symbol: Fonts.Icons.mdiPalette
            text: qsTr("Color")
            menu: Menues.ColorMenu {
                anchors.centerIn: parent
                item: stackView.hasColor ? stackView.currentItem.item : null
                parent: window.contentItem
            }
            visible: menu.item !== null
        },

        ToolBarAction {
            symbol: Fonts.Icons.mdiPalette
            text: qsTr("Color")
            visible: {
                return stackView.currentItem
                        && typeof (stackView.currentItem["selectColor"]) === "function"
            }
            onTriggered: stackView.currentItem.selectColor()
        },

        ToolBarAction {
            symbol: Fonts.Icons.mdiLabel
            text: qsTr("Add Tag")
            visible: stackView.currentItem
                     && (typeof (stackView.currentItem["addTag"]) === "function")
            onTriggered: stackView.currentItem.addTag()
        },

        ToolBarAction {
            symbol: Fonts.Icons.mdiAttachment
            text: qsTr("Attach File")
            visible: stackView.currentItem
                     && (typeof (stackView.currentItem["attach"]) === "function")
            onTriggered: stackView.currentItem.attach()
        },

        ToolBarAction {
            id: searchToolButtonAction

            symbol: Fonts.Icons.mdiSearch
            text: qsTr("Search")
            visible: stackView.currentItem
                     && (typeof (stackView.currentItem["find"]) === "function")
            onTriggered: stackView.currentItem.find()
        },

        ToolBarAction {
            symbol: Fonts.Icons.mdiSort
            text: qsTr("Sort")
            visible: stackView.currentItem
                     && typeof (stackView.currentItem.sort) === "function"
            onTriggered: stackView.currentItem.sort()
        },

        ToolBarAction {
            symbol: Fonts.Icons.mdiCalendarToday
            text: qsTr("Set Due Date")
            visible: stackView.currentItem
                     && typeof (stackView.currentItem.setDueDate) === "function"
            onTriggered: stackView.currentItem.setDueDate()
        },

        ToolBarAction {
            symbol: Fonts.Icons.mdiDelete
            text: qsTr("Delete")
            visible: stackView.currentItem
                     && typeof (stackView.currentItem["deleteItem"]) === "function"
            onTriggered: stackView.currentItem.deleteItem()
        },

        ToolBarAction {
            symbol: Fonts.Icons.mdiRemoveDone
            text: qsTr("Delete Completed Items")
            visible: stackView.currentItem
                     && typeof (stackView.currentItem["deleteCompletedItems"]) === "function"
            onTriggered: stackView.currentItem.deleteCompletedItems()
        },

        ToolBarAction {
            symbol: Fonts.Icons.mdiPublishedWithChanges
            text: qsTr("Set Progress")
            visible: stackView.currentItem
                     && typeof (stackView.currentItem["setProgress"]) === "function"
            onTriggered: stackView.currentItem.setProgress()
        }
    ]

    Connections {
        target: shortcuts.findShortcut
        function onActivated() {
            searchToolButtonAction.trigger()
        }
    }
}
