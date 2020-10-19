import QtQuick 2.0
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.0

import OpenTodoList 1.0 as OTL

import "../Components"

CenteredDialog {
    id: root

    property OTL.Library library: null
    readonly property alias selectedTodoList: comboBox.currentValue

    function clear() {
        comboBox.currentIndex = -1;
    }

    title: qsTr("Select Todo List")
    modal: true
    width: idealDialogWidth

    footer: DialogButtonBox {
        standardButtons: DialogButtonBox.Ok | DialogButtonBox.Cancel
    }

    Component.onCompleted: d.okButton = footer.button(DialogButtonBox.Ok)

    QtObject {
        id: d

        property AbstractButton okButton: null
    }

    ComboBox {
        id: comboBox

        editable: false
        textRole: "title"
        valueRole: "object"
        width: root.availableWidth
        model: OTL.LibraryItemsModel {
            cache: OTL.Application.cache
            itemType: "TodoList"
        }
    }

    Binding {
        target: comboBox.model
        property: "parentItem"
        value: root.library.uid
        when: root.library
    }

    Binding {
        target: d.okButton
        property: "enabled"
        value: comboBox.currentIndex >= 0
    }
}
