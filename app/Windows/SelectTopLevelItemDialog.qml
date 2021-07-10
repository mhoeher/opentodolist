import QtQuick 2.0
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.0

import OpenTodoList 1.0 as OTL

import "../Components"

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

    footer: DialogButtonBox {
        id: buttons

        standardButtons: DialogButtonBox.Ok | DialogButtonBox.Cancel
        Component.onCompleted: d.okButton = buttons.standardButton(DialogButtonBox.Ok)
    }


    QtObject {
        id: d

        property var okButton: null
    }

    ColumnLayout {
        width: root.availableWidth

        ComboBox {
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

        ComboBox {
            id: topLevelItemComboBox

            editable: false
            textRole: "title"
            valueRole: "object"
            Layout.fillWidth: true
            model: OTL.ItemsModel {
                id: itemsModel
                cache: OTL.Application.cache
                parentItem: libraryComboBox.currentValue ? libraryComboBox.currentValue.uid : ""
            }
        }
    }

    Binding {
        target: d.okButton
        property: "enabled"
        value: topLevelItemComboBox.currentIndex >= 0 && libraryComboBox.currentIndex >= 0
    }
}
