import QtQuick 2.10
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.0

import OpenTodoList 1.0 as OTL

import "../Components" as Components
import "../Utils"
import "../Fonts"
import "../Windows"

ListView {
    id: root

    property Component headerComponent: null
    property Component footerComponent: null

    property Components.ToolButton headerIcon: headerItem ? headerItem.headerIcon : null
    property Components.ToolButton headerIcon2: headerItem ? headerItem.headerIcon2 : null

    property string symbol: Icons.faPlus
    property string symbolFont: Fonts.fontAwesomeSolid.name
    property bool headerItemVisible: true

    property string symbol2
    property string symbolFont2: Fonts.fontAwesomeSolid.name
    property bool headerItem2Visible: false

    property string title
    property bool allowCreatingNewItems: false
    property string newItemPlaceholderText
    property bool allowSorting: true
    property bool allowSettingDueDate: false

    signal headerButtonClicked()
    signal headerButton2Clicked()
    signal todoClicked(var todo)
    signal createNewItem(string title, var args)

    header: Pane {
        property alias headerIcon: headerIcon
        property alias headerIcon2: headerIcon2

        width: parent.width
        height: column.contentHeight

        Column {
            id: column

            width: parent.width

            Loader {
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

                Components.ToolButton {
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

                Components.ToolButton {
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

                Components.TextField {
                    id: newItemTitelEdit
                    Layout.fillWidth: true
                    selectByMouse: true
                    Layout.row: 0
                    Layout.column: 1
                    onAccepted: newItemButton.clicked()
                    placeholderText: root.newItemPlaceholderText
                }

                Components.ToolButton {
                    symbol: Icons.faCalendarAlt
                    visible: root.allowSettingDueDate
                    Layout.row: 0
                    Layout.column: 2
                    onClicked: {
                        dateDialog.selectedDate = newItemRow.dueDate;
                        dateDialog.open();
                    }
                }

                Components.ToolButton {
                    id: newItemButton
                    symbol: Icons.faPlus
                    enabled: newItemTitelEdit.displayText !== ""
                    Layout.column: 3
                    Layout.row: 0
                    onClicked: {
                        var title = newItemTitelEdit.displayText;
                        if (title !== "") {
                            var args = {};
                            if (DateUtils.validDate(newItemRow.dueDate)) {
                                args.dueTo = newItemRow.dueDate;
                                newItemRow.dueDate = new Date("");
                            }
                            root.createNewItem(newItemTitelEdit.displayText, args);
                            newItemTitelEdit.clear();
                            newItemTitelEdit.forceActiveFocus();
                        }
                    }
                }
            }

            Label {
                text: {
                    if (DateUtils.validDate(newItemRow.dueDate)) {
                        return qsTr("Due on: %1").arg(
                                    newItemRow.dueDate.toLocaleDateString());
                    } else {
                        return "";
                    }
                }
                visible: DateUtils.validDate(newItemRow.dueDate)
                Layout.row: 1
                Layout.column: 1
                Layout.columnSpan: 2
                Layout.fillWidth: true
            }

            Components.ToolButton {
                symbol: Icons.faTimes
                onClicked: newItemRow.dueDate = new Date("")
                visible: DateUtils.validDate(newItemRow.dueDate)
                Layout.row: 1
                Layout.column: 3
            }
        }
    }

    footer: Pane {
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

    delegate: TodosWidgetDelegate {
        id: swipeDelegate
        model: root.model
        item: object
        allowSorting: root.allowSorting
        drawSeperator: {
            let result = true;
            if (index == root.model.rowCount() - 1) {
                // Dont draw separator for last item
                result = false;
            } else if (root.section.property !== "") {
                // Check if this is the last item in the current section.
                // Don't draw a separator for it in this case
                let nextItemSection = root.model.data(root.model.index(index + 1, 0),
                                               root.model.roleFromName(root.section.property));
                console.debug(ListView.section, "<=>", nextItemSection, root.section.property);
                if (ListView.section !== nextItemSection) {
                    result = false;
                }
            }
            return result;
        }

        onItemPressedAndHold: showContextMenu({x: 0, y: 0})
        onItemClicked: root.todoClicked(item)

        function showContextMenu(mouse) {
            itemActionMenu.actions = swipeDelegate.itemActions;
            itemActionMenu.parent = swipeDelegate;
            itemActionMenu.popup(mouse.x, mouse.y);
        }

        MouseArea {
            anchors.fill: parent
            acceptedButtons: Qt.RightButton
            onClicked: showContextMenu(mouse)
        }
    }

    QtObject {
        id: d

        property SwipeDelegate openSwipeDelegate: null
    }

    Components.ItemActionMenu {
        id: itemActionMenu
    }
}

