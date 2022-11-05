import QtQuick 2.10
import QtQuick.Layouts 1.1
import QtQuick.Controls.Material 2.0

import OpenTodoList 1.0 as OTL

import "../Components" as Components
import "../Controls" as C
import "../Utils"
import "../Fonts"
import "../Windows"

ListView {
    id: root

    property Component headerComponent: null
    property Component footerComponent: null

    property C.ToolButton headerIcon: headerItem ? headerItem.headerIcon : null
    property C.ToolButton headerIcon2: headerItem ? headerItem.headerIcon2 : null

    property OTL.Library library: null
    property OTL.LibraryItem parentItem: null

    // A function which looks up the library for an item in the list.
    // The default is a function which returns the value of the set library. However, for
    // lists with items from several libraries, this can be set to a function which looks up the
    // library for a particular item.
    property var libraryLookupFunction: function (item, library) {
        return library
    }

    property string symbol: Icons.mdiAdd
    property string symbolFont: Fonts.icons
    property bool headerItemVisible: true
    property bool colorSwatchesVisible: false

    property string symbol2
    property string symbolFont2: Fonts.icons
    property bool headerItem2Visible: false

    property string title
    property bool allowCreatingNewItems: false
    property string newItemPlaceholderText
    property bool allowSorting: true
    property bool allowSettingDueDate: false
    property var hideDueToLabelForSectionsFunction: null
    property bool showParentItemInformation: false

    property var itemsModel: null

    signal headerButtonClicked
    signal headerButton2Clicked
    signal todoClicked(var todo)
    signal createNewItem(string title, var args)
    signal itemSaved(var itemData)
    // Item has been saved (for undo)


    /*
     * WA for https://gitlab.com/rpdev/opentodolist/-/issues/391
     * Background: When the list view is empty, changing the header size will somehow
     * reset the overall scroll. To prevent this, we use a dedicated property to hold the real
     * model. Only if it contains at least one item, we will use it. Otherwise, we inject a
     * placeholder model (and delegate) which ensures the view is never really
     * empty.
     *
     * This WA could be removed as soon as https://bugreports.qt.io/browse/QTBUG-89957
     * is fixed.
     */
    model: {
        if (itemsModel.count === 0) {
            return 1
        } else {
            return itemsModel
        }
    }

    header: C.Pane {
        property alias headerIcon: headerIcon
        property alias headerIcon2: headerIcon2
        property alias item: headerComponentLoader.item

        width: parent.width
        height: column.contentHeight

        Column {
            id: column

            width: parent.width

            Loader {
                id: headerComponentLoader
                width: parent.width
                sourceComponent: root.headerComponent
            }

            RowLayout {
                width: parent.width

                Components.Heading {
                    id: headerLabel

                    level: 2
                    Layout.fillWidth: true
                    font.bold: true
                    text: root.title
                }

                Item {
                    visible: !headerIcon2.visible
                    width: headerIcon2.width
                    height: headerIcon2.height
                }

                C.ToolButton {
                    id: headerIcon2

                    symbol: root.symbol2
                    font.family: root.symbolFont2
                    onClicked: root.headerButton2Clicked()
                    visible: root.headerItem2Visible
                }

                Item {
                    visible: !headerIcon.visible
                    width: headerIcon.width
                    height: headerIcon.height
                }

                C.ToolButton {
                    id: headerIcon

                    symbol: root.symbol
                    font.family: root.symbolFont
                    onClicked: root.headerButtonClicked()
                    visible: root.headerItemVisible
                }
            }

            DateSelectionDialog {
                id: dateDialog
                onAccepted: newItemRow.dueDate = selectedDate
            }

            GridLayout {
                id: newItemRow

                property var dueDate: new Date("")

                columns: 4
                visible: root.allowCreatingNewItems
                width: parent.width

                Item {
                    width: newItemButton.width
                    height: newItemButton.height
                    Layout.row: 0
                    Layout.column: 0
                }

                C.TextField {
                    id: newItemTitelEdit
                    Layout.fillWidth: true
                    Layout.row: 0
                    Layout.column: 1
                    onAccepted: newItemButton.clicked()
                    placeholderText: root.newItemPlaceholderText
                }

                C.ToolButton {
                    symbol: Icons.mdiCalendarToday
                    visible: root.allowSettingDueDate
                    Layout.row: 0
                    Layout.column: 2
                    onClicked: {
                        dateDialog.selectedDate = newItemRow.dueDate
                        dateDialog.open()
                    }
                }

                C.ToolButton {
                    id: newItemButton
                    symbol: Icons.mdiAdd
                    enabled: newItemTitelEdit.displayText !== ""
                    Layout.column: 3
                    Layout.row: 0
                    onClicked: {
                        var title = newItemTitelEdit.displayText
                        if (title !== "") {
                            var args = {}
                            if (DateUtils.validDate(newItemRow.dueDate)) {
                                args.dueTo = newItemRow.dueDate
                                newItemRow.dueDate = new Date("")
                            }
                            root.createNewItem(newItemTitelEdit.displayText,
                                               args)
                            newItemTitelEdit.clear()
                            refocusNewItemEditTimer.restart()
                        }
                    }

                    Timer {
                        id: refocusNewItemEditTimer
                        interval: 1000
                        repeat: false
                        onTriggered: {
                            newItemTitelEdit.forceActiveFocus()
                        }
                    }
                }
            }

            C.Label {
                text: {
                    if (DateUtils.validDate(newItemRow.dueDate)) {
                        return qsTr("Due on: %1").arg(
                                    newItemRow.dueDate.toLocaleDateString())
                    } else {
                        return ""
                    }
                }
                visible: DateUtils.validDate(newItemRow.dueDate)
                Layout.row: 1
                Layout.column: 1
                Layout.columnSpan: 2
                Layout.fillWidth: true
            }

            C.ToolButton {
                symbol: Icons.mdiClose
                onClicked: newItemRow.dueDate = new Date("")
                visible: DateUtils.validDate(newItemRow.dueDate)
                Layout.row: 1
                Layout.column: 3
            }
        }
    }

    footer: C.Pane {
        width: parent.width
        height: footerColumn.contentHeight

        Column {
            id: footerColumn
            width: parent.width

            Loader {
                width: parent.width
                sourceComponent: root.footerComponent
            }
        }
    }

    delegate: model === 1 ? placeholderDelegate : itemDelegate

    Component {
        id: placeholderDelegate

        Components.Empty {}
    }

    Component {
        id: itemDelegate

        TodosWidgetDelegate {
            id: swipeDelegate

            property int updateCounter: 0

            showParentItemInformation: root.showParentItemInformation
            model: root.model
            item: object
            parentItem: root.parentItem
            library: root.libraryLookupFunction(object, root.library)
            leftColorSwatch.visible: colorSwatchesVisible
            leftColorSwatch.color: library.color
            allowSorting: root.allowSorting
            hideDueDate: typeof (root.hideDueToLabelForSectionsFunction)
                         === "function" ? root.hideDueToLabelForSectionsFunction(
                                              ListView.section) : false
            drawSeperator: {
                let result = updateCounter || true
                // Force re-evaluation of binding
                if (index === root.model.rowCount() - 1) {
                    // Dont draw separator for last item
                    result = false
                } else if (root.section.property !== "") {
                    // Check if this is the last item in the current section.
                    // Don't draw a separator for it in this case
                    let nextItemSection = root.model.data(
                            root.model.index(index + 1, 0),
                            root.model.roleFromName(root.section.property))
                    if (ListView.section !== nextItemSection) {
                        result = false
                    }
                }
                return result
            }
            dragTile: itemDragTile
            onSetSwipeDelegate: {
                if (item !== d.openSwipeDelegate) {
                    if (d.openSwipeDelegate) {
                        d.openSwipeDelegate.swipe.close()
                    }
                }
                d.openSwipeDelegate = item
            }

            onItemPressedAndHold: showContextMenu({
                                                      "x": 0,
                                                      "y": 0
                                                  })
            onItemClicked: {
                root.todoClicked(item)
            }
            onItemSaved: root.itemSaved(itemData)

            Connections {
                target: root.model

                function onModelReset() {
                    swipeDelegate.updateCounter++
                }

                function onRowsInserted(parent, first, last) {
                    swipeDelegate.updateCounter++
                }

                function onRowsRemoved(parent, first, last) {
                    swipeDelegate.updateCounter++
                }
            }

            function showContextMenu(mouse) {
                itemActionMenu.actions = swipeDelegate.itemActions
                itemActionMenu.parent = swipeDelegate
                itemActionMenu.popup(mouse.x, mouse.y)
            }

            MouseArea {
                anchors.fill: parent
                acceptedButtons: Qt.RightButton
                onClicked: showContextMenu(mouse)
            }
        }
    }

    QtObject {
        id: d

        property C.SwipeDelegate openSwipeDelegate: null
    }

    Components.ItemActionMenu {
        id: itemActionMenu
    }

    ItemDragTile {
        id: itemDragTile
    }
}
