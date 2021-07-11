import QtQuick 2.0
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.0

import OpenTodoList 1.0 as OTL

import "../Components"

CenteredDialog {
    id: root

    readonly property OTL.Library library: comboBox.currentValue
    property OTL.TodoList initialLibrary: null
    property alias selectInitialTodoList: d.selectInitial

    function clear() {
        comboBox.currentIndex = -1;
        d.findInitial(false);
    }


    title: qsTr("Select Library")
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
        property bool selectInitial: false

        function findInitial(clear = true) {
            if (selectInitial) {
                if (clear) {
                    selectInitial = false;
                }
                for (var i = 0; i < model.count; ++i) {
                    let lib = model.data(model.index(i, 0), OTL.LibrariesModel.LibraryRole);
                    if (lib.uid === root.initialLibrary.uid) {
                        comboBox.currentIndex = i;
                        break;
                    }
                }
            }
        }
    }

    ComboBox {
        id: comboBox

        editable: false
        textRole: "name"
        valueRole: "library"
        width: root.availableWidth
        model: OTL.LibrariesModel{
            id: model
            cache: OTL.Application.cache
            onCountChanged: d.findInitial()
        }
    }

    Binding {
        target: d.okButton
        property: "enabled"
        value: comboBox.currentIndex >= 0
    }
}
