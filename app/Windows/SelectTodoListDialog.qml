import QtQuick 2.0
import QtQuick.Layouts 1.0

import OpenTodoList 1.0 as OTL

import "../Components"
import "../Controls" as C

CenteredDialog {
    id: root

    property OTL.Library library: null
    readonly property alias selectedTodoList: comboBox.currentValue
    property OTL.TodoList initialTodoList: null
    property alias selectInitialTodoList: d.selectInitial

    function clear() {
        comboBox.currentIndex = -1;
        d.findInitial(false);
    }


    title: qsTr("Select Todo List")
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
        property bool selectInitial: false

        function findInitial(clear = true) {
            if (selectInitial) {
                if (clear) {
                    selectInitial = false;
                }
                for (var i = 0; i < model.count; ++i) {
                    let list = model.data(model.index(i, 0), OTL.ItemsModel.ItemRole);
                    if (list.uid === root.initialTodoList.uid) {
                        comboBox.currentIndex = i;
                        break;
                    }
                }
            }
        }
    }

    C.ComboBox {
        id: comboBox

        editable: false
        textRole: "title"
        valueRole: "object"
        width: root.availableWidth
        model: OTL.ItemsModel {
            id: model
            cache: OTL.Application.cache
            itemType: "TodoList"
            onCountChanged: d.findInitial()
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
