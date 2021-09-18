import QtQuick 2.0
import QtQuick.Layouts 1.0

import OpenTodoList 1.0 as OTL

import "../Components"
import "../Controls" as C

CenteredDialog {
    id: root

    property alias library: libraryComboBox.currentValue
    property alias item: topLevelItemComboBox.currentValue
    property alias itemType: itemsModel.itemType

    function clear() {
        topLevelItemComboBox.currentIndex = -1;
        libraryComboBox.currentIndex = -1;
    }


    title: qsTr("Select Item")
    modal: true
    width: idealDialogWidth

    footer: C.DialogButtonBox {
        id: buttons

        standardButtons: C.DialogButtonBox.Ok | C.DialogButtonBox.Cancel
        Component.onCompleted: d.okButton = buttons.standardButton(C.DialogButtonBox.Ok)
    }


    QtObject {
        id: d

        property var okButton: null
    }

    ColumnLayout {
        width: root.availableWidth

        C.ComboBox {
            id: libraryComboBox

            editable: false
            textRole: "name"
            valueRole: "library"
            Layout.fillWidth: true
            model: OTL.LibrariesModel {
                id: libraryModel
                cache: OTL.Application.cache
            }
        }

        C.ComboBox {
            id: topLevelItemComboBox

            editable: false
            textRole: "title"
            valueRole: "object"
            Layout.fillWidth: true
            model: OTL.ItemsSortFilterModel {
                id: itemsSortFilterModel
                sortRole: OTL.ItemsModel.TitleRole
                sourceModel: OTL.ItemsModel {
                    id: itemsModel
                    cache: OTL.Application.cache
                    parentItem: libraryComboBox.currentValue ? libraryComboBox.currentValue.uid : ""
                }
            }
        }
    }

    Binding {
        target: d.okButton
        property: "enabled"
        value: topLevelItemComboBox.currentIndex >= 0 && libraryComboBox.currentIndex >= 0
    }
}
